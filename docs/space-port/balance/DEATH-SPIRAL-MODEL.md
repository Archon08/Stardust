# Interceptor Death-Spiral — Complete Model (real JTL data, both halves)

Joins the two real datasets: turret **coverage** (extracted client `ship_turret.iff`/`ship_chassis.iff`)
and engine **maneuver** (SWGEmu component templates, in-repo). Fully offline; no launch.

## The two real spreads
**Coverage** (285 armed chassis): 223 (~78%) effectively fixed-forward (<6% sphere arc); 48 full turret.
**Maneuver** (126 engine components, absolute caps):
- maxSpeed: min 35.7 · median 58.8 · max 88.5  → **2.48× top-to-bottom spread**
- maxYaw:   45 → 74.5 °/s ; maxPitch similar
- slowest: eng_incom_light_fusialthrust (35.7 / 45) ; fastest: eng_kse_elite (86.7 / 74.6), racing/reward mods ~88.5

## The death-spiral condition (now quantified)
A kill becomes unwinnable-by-the-victim when ALL hold:
1. victim is **fixed-forward** (1 of the 223), AND
2. attacker engine maxSpeed > victim maxSpeed (up to 2.48× possible), AND
3. attacker yaw ≥ victim yaw (up to ~1.65× possible).
Under those, the attacker holds the victim's tail indefinitely; the victim can neither turn into
it (slower yaw) nor fire at it (no rear arc). With the full 2.48× speed / 1.65× yaw spread available
in the same tier pool, a top-engine fixed-forward fighter farms a bottom-engine fixed-forward
fighter with **zero counterplay** — the classic JTL interceptor death-spiral, confirmed in the data.

## Concrete tuning levers (calibration for SpaceBalanceSimTest)
Pick one or combine; numbers are the real caps to clamp against:
- **A. Compress the engine speed spread** within an encounter/tier: cap attacker:victim maxSpeed
  ratio at <=1.5x (vs the 2.48x the data allows). Removes the unrecoverable tail-lock.
- **B. Minimum defensive arc**: give fixed-forward NPC ships a small rear turret (>=6% sphere) so a
  tailing attacker is at least contestable. Shrinks the 223 "defenseless" set.
- **C. Yaw floor for slow hulls**: slow ships (bottom-quartile maxSpeed) get a yaw bonus so they can
  turn-fight even if they can't run — converts a death-spiral into a joust.
Recommended default: **A + C**, tier-gated, so a tier-1 interceptor cannot tail-lock a tier-5 hull.

## Status of the data pipeline (reproducible, offline)
- `tools/fidelity/tre_read.py` + `iff_datatable.py` -> 386 client datatables
- `tools/fidelity/deathspiral.py` -> coverage analysis
- `.github/workflows/component-stats.yml` -> 995 component stat templates (engine/weapon/shield/...)
- Raw SOE client values kept local; SWGEmu component lua already public in-repo; only derived
  findings + tooling committed.

## Next
- Encode levers A/C as the real constants in `SpaceBalanceSimTest` and assert the spiral is removed.
- Per-tier component min/max absolute ranges (for full crafting-aware balance) need the loot-value
  normalization pass (the second stat block is the per-level fraction) — a further offline step.
