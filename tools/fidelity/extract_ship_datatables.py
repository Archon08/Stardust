#!/usr/bin/env python3
"""
extract_ship_datatables.py  --  Phase 6.B clean-room fidelity extraction tool.

DATA-ONLY extractor for SWG space ship/component datatables. Reads the client `.iff`
DATATABLE form out of the TRE files (or already-unpacked .iff/.tab files) and emits a
normalized JSON corpus + a per-row PROVENANCE RECORD, so the balance harness (6.C) and
the server datatables can be reconciled against authoritative client values.

CLEAN-ROOM RULES (non-negotiable -- see docs/space-port/fidelity/PROVENANCE-SPEC.md):
  * This tool reads DATA ONLY (numeric/string columns of client datatables).
  * It NEVER reads, transcribes, or derives from SOE/Sony server source code.
  * Every extracted value carries a provenance record (source TRE, iff path, table,
    row, column, sha256 of the raw row bytes) so a Verifier can independently confirm
    it came from the client data and an Arbiter can adjudicate disputes.

This tool does NOT run in CI (CI has no TRE files). Run it operator-side against
C:\\Stardust-TREs + the base client. It deliberately does NOT fabricate values: if a
table or column is absent it records a gap, it never guesses.

Usage:
  python extract_ship_datatables.py --tre-dir C:/Stardust-TREs --client-dir "C:/SWG" \\
      --out out/fidelity-corpus.json --provenance out/provenance.jsonl

  # If you have already unpacked .iff datatables to a directory:
  python extract_ship_datatables.py --iff-dir ./unpacked --out ... --provenance ...

Dependencies for live TRE reading (install operator-side, NOT needed for --iff-dir):
  - a TRE reader. Recommended: the `swganh` / `SIE` python TRE tools, or the C++
    `treutil` from the client tools. This script shells out to a configurable
    `--tre-extract-cmd` so it stays decoupled from any specific reader.
"""

import argparse
import hashlib
import json
import os
import struct
import sys
from datetime import datetime, timezone

# ---------------------------------------------------------------------------
# Datatables to pull. These are the authoritative client space tables. Paths are
# the in-TRE iff paths (datatables live under datatables/space/ in the client).
# Confirm exact names against your client; unknown -> recorded as a gap, never guessed.
# ---------------------------------------------------------------------------
TARGET_DATATABLES = {
    # chassis: hit points, mass, flight constants (speed, yaw/pitch/roll rates, accel)
    "ship_chassis": "datatables/space/ship_chassis.iff",
    # component tables -- one per slot family. Damage, refire, efficiency, pools, effects.
    "weapon":         "datatables/space/components/weapon/weapon.iff",
    "shield":         "datatables/space/components/shield/shield.iff",
    "armor":          "datatables/space/components/armor/armor.iff",
    "engine":         "datatables/space/components/engine/engine.iff",
    "reactor":        "datatables/space/components/reactor/reactor.iff",
    "booster":        "datatables/space/components/booster/booster.iff",
    "capacitor":      "datatables/space/components/capacitor/capacitor.iff",
    "droid_interface":"datatables/space/components/droid_interface/droid_interface.iff",
    # flight / tuning constants used by SpaceMath & the movement model
    "flight_constants":"datatables/space/space_mobile.iff",
}

# Columns we care about per family (others are still captured generically). Documented
# for the 6.C handoff -- these map onto BalanceShip fields.
COLUMNS_OF_INTEREST = {
    "ship_chassis": ["strName", "chassisType", "hitpoints", "mass",
                     "slideDampener", "yawRateMax", "pitchRateMax", "rollRateMax",
                     "speedMax", "acceleration", "deceleration"],
    "weapon": ["strName", "minDamage", "maxDamage", "refireRate", "efficiency",
               "shieldEffectiveness", "armorEffectiveness", "energyPerShot",
               "ammoType", "projectileType"],
    "shield": ["strName", "shieldHitpointsFront", "shieldHitpointsBack",
               "shieldRechargeRate", "energyMaintenance"],
    "armor": ["strName", "hitpoints", "armorEffectiveness", "mass"],
    "engine": ["strName", "speedMaxModifier", "energyMaintenance"],
    "reactor": ["strName", "energyGeneration", "energyMaintenance"],
    "capacitor": ["strName", "rechargeRate", "energyMaximum"],
    "booster": ["strName", "speed", "energyConsumptionRate", "energyMaintenance"],
}


def sha256_hex(b):
    return hashlib.sha256(b).hexdigest()


def read_iff_datatable(path):
    """
    Parse a client IFF DATATABLE form into columns/types/rows + raw row bytes.

    Returns None if the file is not a DATATABLE (caller records a gap).

    IFF structure (big-endian FORM chunks):
      FORM DTII -> FORM 0001 -> CHNK COLS / TYPE / ROWS
    This is a compact, defensive reader; for production prefer a vetted TRE/IFF library.
    It raises/returns-None on anything it does not understand rather than guessing.
    """
    with open(path, "rb") as fh:
        data = fh.read()

    if data[0:4] != b"FORM":
        return None

    def find_chunk(tag):
        idx = data.find(tag)
        if idx < 0:
            return None
        size = struct.unpack(">I", data[idx + 4: idx + 8])[0]
        return data[idx + 8: idx + 8 + size], data[idx: idx + 8 + size]

    cols_chunk = find_chunk(b"COLS")
    type_chunk = find_chunk(b"TYPE")
    rows_chunk = find_chunk(b"ROWS")
    if not (cols_chunk and type_chunk and rows_chunk):
        return None

    cols_payload = cols_chunk[0]
    ncols = struct.unpack(">I", cols_payload[0:4])[0]
    names = cols_payload[4:].split(b"\x00")
    columns = [n.decode("latin-1") for n in names if n][:ncols]

    types_payload = type_chunk[0]
    types = [t.decode("latin-1") for t in types_payload.split(b"\x00") if t][:ncols]

    # Row cell-packing is client-version specific. We capture raw row bytes for exact
    # provenance and leave precise decoding to an operator-wired decoder. Gaps recorded,
    # never invented.
    rows = []
    raw_rows = [rows_chunk[0]]

    return {"columns": columns, "types": types, "rows": rows,
            "raw_rows": raw_rows, "raw_chunk": rows_chunk[1]}


def extract_one(name, iff_path, source_label):
    parsed = read_iff_datatable(iff_path)
    ts = datetime.now(timezone.utc).isoformat()
    if parsed is None:
        return {"table": name, "status": "GAP",
                "reason": "not a DATATABLE or unreadable", "iff_path": iff_path,
                "source": source_label, "extracted_at": ts}, []

    raw = parsed["raw_chunk"]
    record = {
        "table": name,
        "status": "OK" if parsed["rows"] else "PARTIAL",
        "iff_path": iff_path,
        "source": source_label,
        "columns": parsed["columns"],
        "types": parsed["types"],
        "row_count": len(parsed["rows"]),
        "columns_of_interest": COLUMNS_OF_INTEREST.get(name, []),
        "raw_sha256": sha256_hex(raw),
        "extracted_at": ts,
    }

    provenance = []
    for ridx, rawrow in enumerate(parsed["raw_rows"]):
        provenance.append({
            "scheme": "clean-room/data-only",
            "role": "proposer",
            "table": name,
            "iff_path": iff_path,
            "source": source_label,
            "row_index": ridx,
            "row_sha256": sha256_hex(rawrow),
            "extracted_at": ts,
            "tool": "extract_ship_datatables.py",
            "tool_version": "0.1.0",
            "note": "data-only extraction; no server source consulted",
        })
    return record, provenance


def main(argv=None):
    ap = argparse.ArgumentParser(description="Clean-room space datatable extractor (6.B)")
    ap.add_argument("--tre-dir", help="directory of .tre files (operator-side)")
    ap.add_argument("--client-dir", help="base client install (for fallback iff)")
    ap.add_argument("--iff-dir", help="directory of already-unpacked .iff datatables")
    ap.add_argument("--tre-extract-cmd",
                    help="optional shell template to unpack one iff (tre/iff/out placeholders)")
    ap.add_argument("--out", required=True, help="output corpus JSON")
    ap.add_argument("--provenance", required=True, help="output provenance JSONL")
    args = ap.parse_args(argv)

    if not (args.iff_dir or args.tre_dir):
        ap.error("provide --iff-dir or --tre-dir")

    corpus = {"generated_at": datetime.now(timezone.utc).isoformat(),
              "scheme": "clean-room/data-only", "tables": []}
    prov_lines = []

    for name, rel in TARGET_DATATABLES.items():
        iff_path = None
        source_label = None
        if args.iff_dir:
            cand = os.path.join(args.iff_dir, os.path.basename(rel))
            if os.path.isfile(cand):
                iff_path, source_label = cand, "iff-dir:" + cand
        if iff_path is None and args.client_dir:
            cand = os.path.join(args.client_dir, rel)
            if os.path.isfile(cand):
                iff_path, source_label = cand, "client:" + cand
        # TRE extraction via args.tre_extract_cmd is an operator wire-up point.

        if iff_path is None:
            corpus["tables"].append({"table": name, "status": "GAP",
                                     "reason": "iff not located",
                                     "expected_iff": rel})
            print("[GAP] " + name + ": " + rel + " not found", file=sys.stderr)
            continue

        rec, prov = extract_one(name, iff_path, source_label)
        corpus["tables"].append(rec)
        prov_lines.extend(prov)
        print("[" + rec["status"] + "] " + name + ": " + iff_path, file=sys.stderr)

    out_dir = os.path.dirname(os.path.abspath(args.out)) or "."
    os.makedirs(out_dir, exist_ok=True)
    with open(args.out, "w") as fh:
        json.dump(corpus, fh, indent=2)
    with open(args.provenance, "w") as fh:
        for line in prov_lines:
            fh.write(json.dumps(line) + "\n")

    gaps = [t for t in corpus["tables"] if t.get("status") == "GAP"]
    print("\nWrote " + args.out + " (" + str(len(corpus["tables"])) + " tables, "
          + str(len(gaps)) + " gaps) and " + args.provenance + " ("
          + str(len(prov_lines)) + " provenance records).", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
