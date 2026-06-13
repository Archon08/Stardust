# Original SWG Data Corpus — Research Catalog

Goal: make space **Live-faithful** using **original SWG data** — NOT custom balance. This catalogs
every authentic-SWG data source available and identifies which holds the authoritative values.

## A. Authoritative ORIGINAL space data — the extracted client datatables (SOE Live)
Extracted offline from the JTL client TRE overlay (`tools/fidelity/`). 386 `datatables/space` tables.
These ARE original SOE values — use directly, do not re-balance.

| Table | Rows | Authoritative content |
|---|---|---|
| ship_chassis.iff | 313 | every ship's slot layout + per-slot hit-weights + targetable flags |
| ship_chassis_<ship>.iff | 294 | per-ship slot→component compatibility |
| ship_components.iff | 1,115 | component roster → type + compatibility class |
| ship_weapon_components.iff | 179 | projectile range (256-1024) + speed |
| missiles.iff | 21 | speed, acquisition time/angle, countermeasure difficulty/time-factor |
| countermeasures.iff | 7 | count, speed, scatter angle |
| ship_turret.iff | 312 | per-chassis turret yaw/pitch arcs |
| asteroidfield/*.iff | 55 | per-zone field centers/radius/density/seed (14 numeric cols) |
| nebula/*.iff | 12 | per-zone nebula volumes (42 cols: pos/radius/density/color/damage) |
| spacestation/*.iff | 8 | per-zone station coordinates |
| hyperspace/*.iff | 2 | 32 hyperspace + 5 system jump locations |
| formation/*.iff | 4 | AI squadron formation offsets |

## B. Component numeric stats (HP/damage/energy/maneuver) — SWGEmu RE'd Live values
NOT in the client datatables (client ships only `shared_*` appearance). They are SWGEmu's
reverse-engineered Live values, already in-repo as Lua templates:
`bin/scripts/object/tangible/ship/components/{engine,reactor,shield_generator,armor,weapon,booster,
weapon_capacitor,droid_interface}/*.lua` — 995 stat-bearing templates (harvested via
`.github/workflows/component-stats.yml`). Each carries the absolute caps (e.g. engine maxSpeed/yaw/
pitch/roll) + per-level fractions. This is the canonical SWGEmu reconstruction every SWGEmu/Stardust
server already runs on — i.e. the authentic dataset, not something to invent.

## C. Live server snapshot — validation source
Google Drive: "Last Live Stardust 1 SQL Dump" — a real running-server database (actual instantiated
ships/components with rolled stats). Use to VALIDATE the RE'd values against what a live server held,
not as the base definition.

## D. Original SWG GROUND data (parsed) — for completeness
`project-references/.../swg-data/DT_SWG_*.csv` (8 tables): Creatures (1.9MB), Resources (1MB),
Schematics (1MB), SkillBoxes, LootGroups/Items, Regions, Commands. Parsed original SWG ground data.

## E. Client art — Starborne only
`ProjectFulcrum/Content/SWG_Assets` = 45,308 `.uasset` (full SWG client art converted to UE5). Not
used by the Core3 server; relevant to Starborne, not this port.

## Principle (corrected): Live-faithful, no custom balance
The port must use A + B as-is. The interceptor death-spiral, IF present in this original data, is
**period-correct Live behavior** — preserve it, do not "fix" it with invented tuning. The earlier
DEATH-SPIRAL-MODEL tuning levers are therefore advisory-only / NOT to be applied: they described how
one *could* re-balance, but the directive is fidelity to original SWG.

## What we have vs what's missing
- HAVE (authoritative): all space geometry/timing/encounter data (A); RE'd component stats (B);
  Live validation snapshot (C); ground data (D).
- The only true "original SOE server-side" gap is the raw component object-template numbers, which
  SWGEmu reconstructed (B) and the Live snapshot (C) corroborates. For Live-faithfulness that chain
  (SWGEmu RE + Live snapshot validation) is the accepted standard.
