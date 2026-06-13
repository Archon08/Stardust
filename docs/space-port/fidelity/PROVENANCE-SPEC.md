# Clean-Room Fidelity Extraction — Provenance Spec (Phase 6.B)

Status: **design + tooling ready**. The extraction tool is committed at
`tools/fidelity/extract_ship_datatables.py`; this document is the contract it implements.
Execution requires the client TRE files (operator-side `C:\Stardust-TREs` + base client)
and therefore **does not run in CI**.

## Goal

Recover authoritative SWG space ship/component stat curves from the **client** datatables
so the balance harness (6.C) and the server datatables can be reconciled against ground
truth — under a clean-room discipline that keeps us legally and technically clean.

## The clean-room scheme: Proposer / Verifier / Arbiter

Three roles, three separations of concern. No single party both reads server source and
writes our values.

| Role | Input it MAY touch | Output | Must NOT touch |
|---|---|---|---|
| **Proposer** | Client `.iff` datatables (DATA only) | Candidate value + provenance record | SOE/Sony server source |
| **Verifier** | Proposer's provenance record + an independent re-read of the same client data | PASS/FAIL on each value (hash + value match) | The Proposer's tooling internals |
| **Arbiter** | Both records when Verifier FAILs | Final adjudicated value or `DISPUTED` gap | — |

The point: every number we ship is traceable to a client-data byte range, independently
re-derivable, and never sourced from server code we are reimplementing.

### Hard rules

1. **DATA-ONLY.** Only numeric/string/enum columns of client datatables are extracted.
   Never read, transcribe, paraphrase, or derive from SOE/Sony **server** source code.
2. **No fabrication.** A missing table/column/row is recorded as a `GAP`, never guessed.
   The tool's smoke run against an empty input yields 10 GAPs and 0 values — by design.
3. **Provenance on every value.** No value enters the corpus without a provenance record.
4. **Independent verification.** The Verifier re-reads the raw client bytes and must match
   the Proposer's `row_sha256`. Hash mismatch == automatic FAIL == Arbiter.
5. **Append-only audit.** Provenance JSONL is append-only; corrections add records, never
   rewrite history.

## Provenance record format (JSONL, one object per row)

```json
{
  "scheme": "clean-room/data-only",
  "role": "proposer",
  "table": "weapon",
  "iff_path": "datatables/space/components/weapon/weapon.iff",
  "source": "client:C:/SWG/datatables/space/components/weapon/weapon.iff",
  "row_index": 17,
  "row_sha256": "<sha256 of raw row bytes>",
  "extracted_at": "2026-06-13T09:50:00+00:00",
  "tool": "extract_ship_datatables.py",
  "tool_version": "0.1.0",
  "note": "data-only extraction; no server source consulted"
}
```

Verifier records reuse the schema with `"role": "verifier"` plus `"result": "PASS|FAIL"`
and `"verifier_row_sha256"`. Arbiter records add `"role": "arbiter"`,
`"decision": "value|DISPUTED"`, and `"rationale"`.

## Datatables to pull (the extraction target list)

From `tools/fidelity/extract_ship_datatables.py :: TARGET_DATATABLES`:

| Family | Expected client iff | Key columns -> BalanceShip field |
|---|---|---|
| `ship_chassis` | `datatables/space/ship_chassis.iff` | hitpoints->chassisHealth, speedMax->speedMax, yawRateMax->yawRate, mass |
| `weapon` | `datatables/space/components/weapon/weapon.iff` | minDamage/maxDamage->weaponMin/Max, refireRate->refireSeconds, efficiency->weaponEfficiency, shield/armorEffectiveness |
| `shield` | `datatables/space/components/shield/shield.iff` | shieldHitpointsFront/Back->shieldFront/Rear, rechargeRate |
| `armor` | `datatables/space/components/armor/armor.iff` | hitpoints->armorFront/Rear + componentHealth, armorEffectiveness |
| `engine` | `.../engine/engine.iff` | speedMaxModifier |
| `reactor` | `.../reactor/reactor.iff` | energyGeneration/Maintenance |
| `booster` | `.../booster/booster.iff` | speed, energyConsumptionRate |
| `capacitor` | `.../capacitor/capacitor.iff` | rechargeRate, energyMaximum |
| `droid_interface` | `.../droid_interface/droid_interface.iff` | command tuning |
| `flight_constants` | `datatables/space/space_mobile.iff` | global flight tuning for SpaceMath/movement |

Exact paths/column names vary by client build — confirm against the live client; unknowns
become GAPs, not guesses.

## The flight constants

`SpaceMath` / `ShipObjectTransform` use tuning constants (max velocity, delta clamps,
rotation epsilons — see `ShipObjectTransform::VELOCITY_MAX`, `DELTA_MAX/MIN`,
`POSITION_EPSILON`, `ROTATION_EPSILON`). Where these are data-driven they come from
`space_mobile.iff` / chassis rows; where they are server constants they are **already in
our ported source** and are NOT re-extracted (extracting them would be re-deriving server
behavior, which the clean-room rules forbid). The spec records which constants are
data-sourced vs code-sourced so the boundary stays auditable.

## How the corpus feeds 6.C

`fidelity-corpus.json` -> map columns onto `space_balance::BalanceShip` fields (mapping in
the table above and the `BalanceShip` struct comments) -> replace the synthetic
`archetypes()` in `SpaceBalanceSimTest.cpp` with real per-chassis rows -> the win-rate
matrix and death-spiral probe then reflect REAL balance, and the dominant-strategy check
can be promoted from report-only to a hard CI gate.

## Running it (operator-side)

```sh
python tools/fidelity/extract_ship_datatables.py \
  --client-dir "C:/path/to/SWG" \
  --tre-dir "C:/Stardust-TREs" \
  --out out/fidelity-corpus.json \
  --provenance out/provenance.jsonl
```

Then run an independent Verifier pass (second operator / second reader) and diff the
`row_sha256` values. Any FAIL goes to the Arbiter.
