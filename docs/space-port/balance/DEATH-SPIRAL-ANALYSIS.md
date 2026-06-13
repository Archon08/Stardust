# Interceptor Death-Spiral — Structural Analysis (real JTL data)

Derived from the extracted client datatables (`ship_turret.iff`, `ship_chassis.iff`,
`ship_weapon_components.iff`) via `tools/fidelity/`. Aggregate findings only; raw SOE values local.

## The structural finding
Of **285 armed chassis**:
- **223 (~78%) are effectively fixed-forward** — combined turret coverage <6% of the sphere.
  They can only fire at targets roughly ahead; a faster attacker that gets on their tail cannot
  be engaged at all.
- **48 (~17%) are turret-protected** — full-sphere coverage (corvette/decimator/imperial_gunboat
  carry 6-8 turreted hardpoints at 100% coverage).

This is the *structural* root of the interceptor death-spiral: the overwhelming majority of armed
ships depend entirely on their own speed/maneuver to avoid being tail-chased, because they have no
defensive firing arc. Once out-maneuvered, they are farmable with zero counterplay.

## Coverage classes (real arc data, ship_turret.iff)
| class | yaw / pitch | population |
|---|---|---|
| full turret | ±180° / −90°..0° | 41 slots |
| narrow fixed | ±45° / −30°..+20° | 32 slots |
| (others) | mixed | remainder |

## Weapon engagement bands (ship_weapon_components.iff)
Ranges present: 256 / 400 / 512 / 768 / 1024.

## The other half (needs the server-side stat layer)
This quantifies the *coverage* half. The *maneuverability* half (which fixed-forward ships are too
slow to turn-fight) requires per-component engine stats (speed, yaw/pitch/roll rates). Those are NOT
in the client TRE overlay (client ships only `shared_*` appearance templates); they are server-side
data in the SWGEmu schematic/content layer (already in-repo under `object/draft_schematic/space/` +
`object/tangible/ship/components/`). Joining engine rates to this coverage map yields the full
death-spiral model + concrete per-chassis tuning. That join is the next step (fully offline).

## Tuning levers this exposes (for SpaceBalanceSimTest, once engine stats join)
- Minimum defensive arc: grant fixed-forward NPC ships a small rear-defense turret, or
- Cap interceptor closing-speed advantage vs fixed-forward classes, or
- Tier-gate the speed delta so a tier-1 interceptor can't tail-lock a tier-5 freighter.
