# 01 — Receipts: Evidence Base

All findings verified against live trees: `swgemu/Core3@unstable` (active through 2026-05),
`swgemu/engine3@master`, `Archon08/Stardust@unstable` (HEAD `a4e475d`, 2019-06-30).

## A. The four 2019 blockers — all solved upstream

From TheAnswer's 2019 "1.0 and the future of JTL" thread, mapped to live code:

| 2019 Blocker | Status | Evidence (repo paths in Core3 unless noted) |
|---|---|---|
| Wrong dimensionality — JTL needs true 3D, Core3 is 2.5D quadtree | **SOLVED** | engine3: `MMOEngine/src/engine/util/u3d/BasicOcTree.{cpp,h}` — real 8-octant 3D index. Core3 wraps it: `src/server/zone/Octree.{cpp,h}`, `OctreeReference`, `ActiveAreaOctree`. `SpaceZoneImplementation` backs all range queries with `octree->inRange(x, y, z, range, ...)` |
| "Second engine + bridge" required for space | **SOLVED — cleaner than proposed** | `Zone.idl` refactored to abstract base (`class Zone extends SceneObject`, Zone.idl:42) with abstract `getSpaceManager()`, `isSpaceZone()`. Split: `GroundZone.idl` (226 ln) / `SpaceZone.idl` (160 ln). Both share the SceneObject graph, network layer, scheduler. One engine. |
| Coordinate/orientation mismatch (x/z/y vs ground) | **SOLVED** | `objects/ship/transform/`: `SpaceMath.{cpp,h}`, `ShipObjectTransform`, `SpaceTransform.h` — Vector3 + Quaternion, prev/current/next interpolation state, world-to-local conversion |
| "Core3 wasn't designed for ticks" — projectile physics nightmare | **SOLVED — sidestepped** | No per-frame loop. `SpaceMath::getIntersection` ray/segment-vs-mesh against AppearanceTemplate; `predictTransform(transform, timeSec)` lead prediction; client-prediction + server-validation (SWG-faithful) |

## B. Core3-2026 space subsystem inventory (what we are porting)

### C++ — ~272 files (37 IDL), est. 60-80k lines, cleanly clustered

| Cluster | Path (under `MMOCoreORB/src/`) | Files | Notes |
|---|---|---|---|
| Zone split + octree | `server/zone/` top level | ~14 | `Zone.idl`/`GroundZone.idl`/`SpaceZone.idl` + impls, `Octree.*`, `OctreeReference.*`, `ActiveAreaOctree.*`, `SpaceZoneContainerComponent`, `SpaceZoneLoadManagersTask.h` |
| Ship manager | `server/zone/managers/ship/` | 14 | `ShipManager`, `ShipAgentTemplateManager`, `SpaceSpawnObserver.idl`, spawn groups, timer tasks |
| Space manager | `server/zone/managers/space/` | 4 | `SpaceManager.idl`+impl, `SpaceAiMap.h`, `SpaceRegionMap.h` |
| Collision | `server/zone/managers/spacecollision/` | 3 | `SpaceCollisionManager`, `SpaceCollisionResult` |
| Combat | `server/zone/managers/spacecombat/` | 7 | `SpaceCombatManager`; projectile/missile/countermeasure |
| Ship objects | `server/zone/objects/ship/` | 40 | `ShipObject.idl` (1,942 ln) + impl (2,787 ln), Fighter/Pob/MultiPassenger, chassis/collision/delta/target/permission data |
| Ship AI | `objects/ship/ai/` (+btspace/, events/) | ~52 | `ShipAiAgent.idl`+impl (2,564 ln), CapitalShip, SpaceStation, squadrons, full parallel behavior-tree stack (btspace/decoratorspace/leafspace/nodespace) |
| Components | `objects/ship/components/` | 35 | 12 component IDLs: armor/booster/capacitor/chassis/countermeasure/droid_interface/engine/missile/reactor/shield/sub/weapon |
| Events/squadron/transform | `objects/ship/{events,squadron,transform}/` | 21 | Hyperspace/docking/recovery tasks, formation logic, SpaceMath |
| Space areas/regions | `objects/{area,region}/space/` | 9 | Nebula, SpaceActiveArea, SpaceRegion, SpaceSpawnArea |
| Other objects | scattered | ~20 | `ShipControlDevice.idl`, `ShipDeed.idl`, `SpaceshipTerminal.idl`, repair kits, interior components, paint kits, `staticobject/SpaceObject.idl` |
| Packets | `server/zone/packets/{jtl,ship}/` + object/ | 31 | transforms (packed pos/quat/velocity/rotation-rate), `OnShipHit`, hyperspace, projectile/missile messages |
| Commands | `objects/creature/commands/` | 16 | Hyperspace, LaunchIntoSpace, Pilot/UnpilotShip, component install/repair, SpaceQueueCommand |
| Templates/terrain | `src/templates/tangible/ship/`, `src/terrain/` | 6 | chassis/component/deed templates, `SpaceTerrainAppearance` |
| Tests | `src/tests/` | 2 | `SpaceZoneTest.cpp`, `LuaShipAgentTest.cpp` |

### Lua — ~6,443 files (~5,900 are generated data templates; ~150 hand-written logic)

| Dir (under `MMOCoreORB/bin/scripts/`) | Files | Contents |
|---|---|---|
| `ai_space/` | 7 | behavior-tree templates: default, escort, turretship, waveAttack, stations |
| `managers/space/` | 17 | space_manager, ship_chassis, ship_pilot, ship_turret + `regions/` x10 zones |
| `ship_mobile/` | 1,944 | **1,925 NPC ship loadouts** + patrol points per system |
| `mobile/space/` + conversations | 128 | quest givers, squadron greeters |
| `screenplays/space/` | ~100 | 18 dynamic mission types, `SpaceQuestLogic.lua`, **9 squadron chains** (Havoc, CrimsonPhoenix, Vortex / BlackEpsilon, Inquisition, Storm / Corsec, RSF, Smuggler), per-zone spawners, station conv handlers |
| `object/ship/**`, `object/tangible/ship/**` | ~1,850 | ship + component templates |
| `object/{static,tangible}/space/**` | ~205 | asteroids, debris, stations, mission objects |
| `object/draft_schematic/space/` | ~335 | shipwright crafting |
| `loot/{groups,items}/{ship,space}/` | ~1,145 | component loot, faction loot |

### Config

- `bin/conf/config.lua:129-147` — `SpaceZonesEnabled` = 10 zones: corellia, dantooine, dathomir,
  endor, heavy1, light1, lok, naboo, tatooine, yavin4 (full Live system list)
- `src/conf/ConfigManager.h` — `Core3.JTL.JTLEnabled` (**default false**), `Core3.JTL.LaunchFromDevice`,
  `Core3.Regions.DisableSpaceSpawns`, `minimumSpaceSpawnInterval`, `spaceSpawnCheckRange`

### engine3 dependency

- Core3 pins engine3 as shallow submodule: `.gitmodules` → `MMOCoreORB/utils/engine3`, currently `7012c03`
- Space-relevant engine pieces: `engine/util/u3d/BasicOcTree.{cpp,h}`, `BasicOcTreeNode.h`,
  `OcTreeEntryInterface.h` (+ the TreeEntry abstraction the Zone split depends on)

## C. Stardust-2019 audit (what we are porting into)

- **Branch `unstable`, HEAD `a4e475d`, 2019-06-30.** Fork of `andmer/Stardust` (itself Core3-derived). ~282 MB.
- **Zone:** monolithic concrete `Zone.idl`, QuadTree only (`QuadTree.{h,cpp}`, `QuadTreeEntry.idl`),
  `CloseObjectsVector` with 2D semantics. **No octree anywhere. No Space/Ground split.**
- **Ship stub:** `objects/ship/` = 7 vanilla files (ShipObject, FighterShipObject, SpaceStationObject
  idl+impl, ShipComponent.h). No Stardust customization detected → replace wholesale.
- **Engine:** no submodule, no vendored engine. `find_package(Engine3 REQUIRED)` → external 2019-era
  PublicEngine (ZonamaDev VM). 2019 engine3 has QuadTree only.
- **Build:** CMake 3.1 / **C++11** / Lua 5.3 / MySQL / Berkeley DB / Java idlc.jar (ships with engine).
  Legacy autotools residue present.
- **Custom ground zones (must survive the Zone split):** chandrila, coruscant, jakku, kaas, moraband, hutta.
- **Custom client stack:** requires ModTheGalaxy TREs + `stardust_s.tre`, `stardust_01-03.tre`,
  22 `mtg_patch_*.tre` (config.lua).
- **Custom content:** `bin/scripts/custom_scripts/` (battle_of_endor, respec_counselor), custom loot
  (infinity, halloween, rngbox, custom), custom mobiles (custom_content, kaas, outfits).
- **Custom C++ (per Starborne docs; exact diff = Phase 0 deliverable):** Mind pool removed, combat
  rebalance, Armorweaver profession, 7 Force-sensitivity levels, badge-based Jedi unlock, PSG
  consumables, world bosses. These live in CreatureObject / CombatManager / PlayerManager /
  SkillManager / DirectorManager — see 03-collision-map.md.
- `config.lua` lists all `space_*` zones **commented out** (vanilla list, never loaded).

## D. What this means

The port is not "add a feature" — it is three nested operations:
1. **Foundation uplift** (engine3 2019→7012c03, C++11→17, idlc regen) — prerequisite, riskiest
2. **Architecture graft** (Zone→Ground/Space split + octree onto a customized 2019 tree)
3. **Subsystem transplant** (the cleanly-clustered ~272 C++ files + ~6,400 Lua files, then
   reconciliation with Stardust combat/profession customizations)

Each is a phase with a gate in 02-port-plan.md.