# Space-Port Balance Simulation Harness (Phase 6.C)

A standalone, **headless** Monte Carlo harness that stress-tests the ported space-combat
math without a running server. It surfaces dominant-strategy breakpoints and quantifies
the **interceptor death-spiral** the space-port plan names as a balance risk.

## Where it lives

| Artifact | Path |
|---|---|
| Harness (gtest) | `MMOCoreORB/src/tests/SpaceBalanceSimTest.cpp` |
| Sample report | `docs/space-port/balance/SAMPLE-REPORT.md` |
| CI runner | `.github/workflows/space-tests.yml` (headless GREEN gate) |

## What it does

It re-implements the **exact** damage pipeline of
`SpaceCombatManager::applyDamage` over a plain-old-data ship model (`BalanceShip`) so
ship/component permutations can be swept in milliseconds:

1. `damage = rand(min..max) * weaponEffect` (weaponEffect clamped `0.1..10`)
2. `damage *= 0.75` for non-player (AI) targets
3. **Shield facing**: `shieldDamage = damage * shieldEffectiveness`; subtract from the
   facing shield; leftover flows on as `leftover / shieldEffectiveness`
4. **Armor**: `armorDamage = damage * armorEffectiveness`; eat the armor pool, then its
   backing component-health pool; leftover flows on as `leftover / armorEffectiveness`
5. **Chassis**: raw leftover subtracts from chassis health; chassis == 0 -> ship dies

Hit chance per shot is a function of maneuver advantage (`yawRate` delta and the penalty
for chasing a faster `speedMax` target). This is the lever that produces the death-spiral:
once an interceptor loses its shields it both takes more chassis damage AND, if out-turned,
lands fewer of its own shots.

## Determinism

Uses a fixed-seed LCG (NOT `System::random`) so win-rate cells are reproducible across CI
runs. Seeds are derived from the `(attacker, defender, trial)` triple.

## Output

`SpaceBalanceSimTest.MonteCarloWinRateMatrix` prints a win-rate matrix (row attacks column)
to stdout, captured by CI in `space-headless.log`. Two further tests gate balance health:

- `InterceptorDeathSpiralIsDetectable` — asserts the light interceptor is the underdog vs
  the heavy gunship in sustained 1v1 attrition (the named death-spiral). If this flips
  above 50%, balance has regressed and CI fails.
- `NoUniversallyDominantArchetype` — fails if any archetype wins >85% against *every*
  other archetype (a single dominant build).

## v1 stat inputs are SYNTHETIC

The archetype stats (Interceptor / Gunship / Bomber) are **clearly-labelled synthetic**
structural inputs — NOT authoritative SWG values. They exist to find *structural*
breakpoints. The `BalanceShip` field names mirror the datatable-backed `ShipObject`
accessors (`weaponMinDamage`, `shieldEffectiveness`, `armorFront`, `chassisHealth`, ...)
so the **real** curves extracted by Phase 6.B (`tools/fidelity/`) plug straight in:
replace `archetypes()` with values read from the extracted `ship_chassis` / `*_component`
datatables.

## Coverage limits (honest notes)

- Models a **1v1** duel. Squadron / N-v-N emergent effects (focus fire, formation,
  reinforcement) are out of scope for v1.
- Per-shot hit chance is a **synthetic geometric proxy**, not a real flight-model
  simulation. True hit rates require the headless-client harness in `SCENARIO-MATRIX.md`.
- Component *destruction* side effects (e.g. losing an engine reduces speed mid-fight) are
  not yet fed back into the maneuver model — a natural v2 once 6.B curves land.
- Countermeasures / missiles / capital-ship subsystem targeting are not modeled in v1.
