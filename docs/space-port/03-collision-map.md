# 03 — Collision Map

Two collision classes: (A) shared Core3 files the space port must modify, and (B) Stardust
custom-delta files the port will land on top of. The intersection of A and B is the danger zone.

## A. Shared-file touchpoints (space-referencing lines in modern Core3)

Counts = lines matching space|ship|jtl|octree in Core3@unstable 2026. These are the files where
the port is a MERGE, not a copy.

| File | ~Lines | What space added | Stardust delta risk |
|---|---|---|---|
| `managers/director/DirectorManager.cpp` | **174** | Lua bindings for ship agents, space spawning, screenplay API | **HIGH** — Stardust custom screenplays hook the same file (battle_of_endor, respec_counselor, world bosses) |
| `objects/scene/SceneObjectImplementation.cpp` | 88 | ship-type checks, space-zone awareness | MEDIUM — possible custom fields |
| `managers/player/PlayerManagerImplementation.cpp` | 68 | space death/ejection, launch/land flow, JTL recruitment | **HIGH** — Stardust profession/Jedi/respec changes live here |
| `ZoneServerImplementation.cpp` | 53 | space zone loading, `startSpaceZones()` | MEDIUM — 6 custom ground zones load here |
| `managers/object/ObjectManager.cpp` | 48 | ship object factory registration | LOW-MEDIUM |
| `objects/creature/CreatureObjectImplementation.cpp` | 44 | pilot state, ship references | **HIGH** — mind-pool removal rewrote attribute arrays here |
| `objects/scene/SceneObject.idl` | 42 | type flags, casts | MEDIUM — IDL change → idlc regen ripple |
| `src/conf/ConfigManager.h` | 22 | `Core3.JTL.*`, space spawn keys | LOW — additive |
| `ZoneServer.idl` | 14 | spaceZones map, range constants | LOW |
| `managers/objectcontroller/command/CommandConfigManager.cpp` | n/a | registers 16 ship commands | LOW — additive registration |
| `objects/intangible/IntangibleObject.idl` + Lua | n/a | ShipControlDevice plumbing | LOW |
| `ZoneProcessServerImplementation.cpp` | 1 | minimal | LOW |

## B. Stardust custom-delta hotspots (from feature list; exact diff = Phase 0.2 deliverable)

| Stardust feature | Files it must have touched | Collides with space? |
|---|---|---|
| Mind pool removed (2-pool HAM) | `CreatureObject.idl`/impl, `CombatManager.cpp`, command costs, client baselines | **YES** — pilot commands, space combat costs, ship operator state all read HAM. Phase 5.1 |
| Combat rebalance (all professions) | `CombatManager.cpp`, weapon/armor logic | Indirect — space combat is a separate manager (`SpaceCombatManager`), but shared damage/defense helpers may be customized |
| Armorweaver profession + skill changes | `SkillManager`, skill Lua, crafting | **YES** at Phase 5.2 — pilot trees enter the same modified skill graph |
| Badge-based Jedi unlock | PlayerManager, badge handling | **YES** — JTL grants badges (ace badges etc.); audit unlock triggers. Phase 5.5 |
| 7 Force-sensitivity levels | PlayerObject, FRS-adjacent code | LOW direct overlap |
| World bosses / events (Collector, Vader, Palpatine) | DirectorManager hooks, custom screenplays | **YES** at DirectorManager merge. Phase 5.4 |
| 6 custom ground zones | config.lua, TREs, zone loading | **YES** at Zone split — must migrate to GroundZone cleanly. Phase 2.2 |
| Custom TRE stack (MTG + stardust_*.tre) | client-side | **YES** as asset risk — space .iff/datatable override check. Phase 4.6 |
| Custom loot (infinity/halloween/rngbox) | loot Lua | Merge-trivial; balance reconciliation only. Phase 4.5 |

## C. Safe-transplant set (no collision expected)

Everything cleanly clustered, confirmed zero Stardust customization:

- `objects/ship/**` — Stardust still has the vanilla 7-file stub → **delete + replace wholesale**
- `managers/{ship,space,spacecollision,spacecombat}/` — directories do not exist in Stardust
- `packets/{jtl,ship}/` — do not exist
- `objects/{area,region}/space/`, space tangibles/intangibles/terminals — do not exist
- `src/templates/tangible/ship/`, `SpaceTerrainAppearance` — do not exist
- All space Lua dirs (`ai_space/`, `managers/space/`, `ship_mobile/`, `screenplays/space/`,
  `object/ship/**`, space loot) — do not exist; only registration files (`serverobjects.lua`,
  config.lua includes) need merge

## D. Foundation collisions (Phase 1, engine-level)

| Area | 2019 Stardust | Modern requirement | Fallout |
|---|---|---|---|
| Spatial entry type | `QuadTreeEntry.idl` (Core3-side) | TreeEntry abstraction (engine3-side) | every SceneObject derivative recompiles; mechanical but wide |
| Engine linkage | external `find_package(Engine3)` | submodule `utils/engine3@7012c03` | CMake rework, ZonamaDev decoupling |
| C++ standard | C++11 | C++17 | language fallout in custom Stardust code too |
| idlc | 2019 idlc.jar | modern idlc | ALL .idl regenerate — Stardust custom IDL edits (CreatureObject etc.) must re-merge into regenerated headers |
| Lua | 5.3 | confirm Core3-2026 pin | likely compatible; verify |

## E. Merge discipline

1. Phase 0.2 delta inventory tags every Stardust-modified file BEFORE any port work; any port
   touching a tagged file requires a three-way merge (vanilla-2019 base, Stardust, Core3-2026),
   never a copy-over.
2. The A∩B files (DirectorManager, PlayerManagerImplementation, CreatureObjectImplementation)
   get dedicated merge sessions with the regression harness run after each.
3. Generated idlc headers are never hand-merged — regenerate from merged .idl only.