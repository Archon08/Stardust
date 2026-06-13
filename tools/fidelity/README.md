# tools/fidelity — Clean-Room Datatable Extraction (Phase 6.B)

DATA-ONLY tooling to recover authoritative SWG space ship/component stats from the
**client** TRE datatables. Design + rules: `docs/space-port/fidelity/PROVENANCE-SPEC.md`.

| File | Purpose |
|---|---|
| `extract_ship_datatables.py` | Proposer-role extractor: reads client `.iff` datatables, emits a normalized JSON corpus + per-row provenance JSONL. Records GAPs; never fabricates. |

## Why this is not in CI

CI has no TRE/client files. Run it operator-side against `C:\Stardust-TREs` + the base
client. The tool is structured so its output (`fidelity-corpus.json`) plugs straight into
the 6.C balance harness (`MMOCoreORB/src/tests/SpaceBalanceSimTest.cpp`).

## Clean-room, in one line

Read client **data** only; never server source; every value carries a verifiable
provenance record (Proposer → Verifier → Arbiter). See the spec.

## Quick start

```sh
python extract_ship_datatables.py --iff-dir ./unpacked \
    --out out/corpus.json --provenance out/prov.jsonl
# or against a live client:
python extract_ship_datatables.py --client-dir "C:/SWG" --tre-dir "C:/Stardust-TREs" \
    --out out/corpus.json --provenance out/prov.jsonl
```

Running with no real data present yields all-GAP output and zero values — the intended
fail-safe (it proves the tool does not invent stats).
