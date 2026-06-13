# Fidelity Extraction — Results (clean-room, real JTL data)

Status: **the offline extraction pipeline is built and the real JTL ship/space datatables
are extracted and parsed.** No game launch, no sudo, no server boot.

## Pipeline (all in tools/, reusable)
1. `unshield` (built from source, no sudo) extracts SWG InstallShield disks → base client TREs.
2. `tools/fidelity/tre_read.py` — SWG TRE (EERT/5000) reader: lists/extracts files from .tre archives.
3. `tools/fidelity/iff_datatable.py` — SWG IFF DATATABLE (DTII) parser → typed rows.

## Where the JTL data actually lives
The base retail disks are pre-JTL (no space data). The JTL ship/space datatables are in the
**MTG client TRE overlay** (`mtg_patch_013_configurable_02.tre` etc.) — confirmed and extracted.

## Extracted & parsed (386 datatables/space tables, 0 parse failures)
| Table | Rows × Cols | Key real fields |
|---|---|---|
| ship_chassis.iff | 313 × 64 | per-slot component class, hit-weights, targetable flags |
| ship_chassis_*.iff (per ship) | 294 files | slot→component compatibility matrices |
| ship_components.iff | 1,115 × 5 | component roster → type (reactor/engine/shield/armor/weapon/booster/capacitor) |
| ship_weapon_components.iff | 179 × 9 | range, projectile speed, ammo, missile/countermeasure/mining/tractor flags |
| missiles.iff | 21 × 15 | server_speed, acquisition seconds/angle, countermeasure difficulty/time-factor |
| countermeasures.iff | 7 × 7 | count, speed, scatter angle |
| ship_turret.iff | 312 × 6 | per-chassis turret yaw/pitch traverse arcs |
| + nebula, asteroidfield, spacestation, formation, hyperspace | ~90 | zone/encounter data |
| component object-templates | 1,116 | per-component stat templates (deeper IFF layer — next) |

## Derived/aggregate findings (safe to record; raw SOE values kept local, not redistributed)
- **Turret-arc distribution** (death-spiral driver): 41 weapon-slots use full ±180° yaw / −90°..0° pitch
  (gunship/turreted), 32 use narrow ±45° / −30°..+20° (fixed-forward fighters). The gap between
  full-coverage turrets and fixed mounts is the structural reason fast interceptors farm slow ships.
- **Missiles**: speed 50–100, acquisition angle 15–20°, countermeasure difficulty 20–90.

## Clean-room note
Raw SOE datatable VALUES are NOT committed to this repo (SWGEmu policy: users supply their own
client). The corpus (386 JSON) is generated locally by the committed tools from the operator's own
client TREs. Only the tooling + aggregate/derived findings live here.

## Next
- Parse the component object-templates (per-component HP / engine maneuver / weapon damage / shield
  recharge) — the deeper IFF SHOT layer.
- Feed real constants into the balance sim (`SpaceBalanceSimTest`) → real interceptor-death-spiral
  quantification + tuning, replacing the synthetic placeholders.
