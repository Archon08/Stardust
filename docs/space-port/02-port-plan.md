# 02 — The Port Plan (Phases 0-6)

Strategy locked: **port the modern Core3 space subsystem INTO the Stardust 2019 tree.**
Every phase ends in a buildable, ground-regression-clean tree. Space stays dark
(`JTLEnabled=false`) until Phase 6 verification passes.

Branching model: `unstable` stays untouched; work lands on `space-port/phase-N` branches,
merging to an integration branch `space-port-integration` after each gate.

---

## Phase 0 — Baseline & Truth (prerequisite for everything)

**Goal:** know exactly what Stardust changed, and prove the tree still builds before touching it.

| # | Work item | Output |
|---|---|---|
| 0.1 | Stand up a build of `unstable` HEAD `a4e475d` with the 2019-era engine3/PublicEngine (ZonamaDev or container recreation) | Reproducible baseline build + tagged image |
| 0.2 | Generate the **Stardust custom delta**: `git diff` vs `swgemu/Core3@unstable` circa 2019-06-30 (merge-base) | `docs/space-port/delta/` — categorized diff inventory: per-file, tagged {combat, profession, jedi, zones, content, infra} |
| 0.3 | Smoke-test harness: server boots, a character logs in, ground combat works, the 6 custom zones (chandrila, coruscant, jakku, kaas, moraband, hutta) load | Scripted regression checklist (becomes the gate test for every later phase) |
| 0.4 | Snapshot DB schema (Berkeley DB object layout) for later migration diffing | Schema notes |

**Gate G0:** baseline builds + smoke tests pass + delta inventory reviewed.
**Effort:** 1-2 iteration-days. The diff categorization is agent-parallelizable.

---

## Phase 1 — Foundation Uplift (engine3 + toolchain)

**Goal:** modern engine under the 2019 tree, behavior unchanged. This is the *highest-risk phase*
and is done FIRST so risk is retired early.

| # | Work item | Notes |
|---|---|---|
| 1.1 | Vendor engine3 as a submodule pinned to `7012c03` (mirroring Core3's `.gitmodules`) | Replaces the external find_package-era engine |
| 1.2 | Toolchain jump: CMake modernization, **C++11 → C++17**, modern idlc | All .idl regenerate — expect wide mechanical churn in generated headers |
| 1.3 | Absorb engine3 API drift 2019→2026: Core dispatch, BasePacket, ORB, TaskManager, **QuadTreeEntry → TreeEntry abstraction** | The TreeEntry refactor is the structural one — it is what lets one entry type live in quad- or octrees |
| 1.4 | Fix compile fallout file-by-file; NO behavior changes permitted in this phase | Agent swarm: parallel fix-compile-error loops are ideal here |
| 1.5 | Re-run Phase 0 smoke harness | Ground behavior must be identical |

**Gate G1 (GO/NO-GO):** tree builds C++17 + engine3@7012c03, full smoke pass.
**Fallback pivot:** if fallout exceeds ~2x budget, STOP — switch to the rebase strategy
(start from Core3@unstable-2026, re-apply the Phase-0 delta inventory on top). The Phase 0
delta inventory is deliberately strategy-neutral so no work is lost on pivot.
**Effort:** 3-7 iteration-days. Least compressible phase alongside POB sync.

---

## Phase 2 — The Zone Split

**Goal:** `Zone` → abstract base + `GroundZone`/`SpaceZone`, octree wired, ground untouched in behavior.

| # | Work item | Source of truth (Core3 paths) |
|---|---|---|
| 2.1 | Port `Zone.idl` abstract base (336 ln) + `ZoneImplementation.cpp` (265 ln) | `src/server/zone/Zone.idl` |
| 2.2 | Port `GroundZone.idl`/impl + `GroundZoneContainerComponent`; migrate ALL existing zone behavior (incl. Stardust's 6 custom zones) into GroundZone | `GroundZone.idl` (226 ln) |
| 2.3 | Port `SpaceZone.idl`/impl + `SpaceZoneContainerComponent` + `SpaceZoneLoadManagersTask` | `SpaceZone.idl`, `SpaceZoneImplementation.cpp` (480 ln) |
| 2.4 | Port Core3 octree wrappers: `Octree.{cpp,h}`, `OctreeReference`, `ActiveAreaOctree` | top-level `server/zone/` |
| 2.5 | `ZoneServer.idl`/impl: `spaceZones` VectorMap, `startSpaceZones()`, range constants (SPACECLOSEOBJECTRANGE=2048, CAPITALSHIPRANGE=8192, SPACESTATIONRANGE=32768) | ~53 changed lines in impl |
| 2.6 | `SceneObject.idl`/impl space-awareness (~42/88 lines) + ZoneComponent split into `GroundZoneComponent`/`SpaceZoneComponent` | `objects/scene/components/` |
| 2.7 | `CloseObjectsVector`/`InRangeObjectsVector` 3D-range semantics | watch for Stardust deltas here from Phase 0 inventory |
| 2.8 | `ConfigManager` space keys (`SpaceZonesEnabled`, `Core3.JTL.*`) — wired but OFF | `src/conf/ConfigManager.h` |

**Gate G2:** ground smoke pass on all zones (vanilla 10 + custom 6); an EMPTY space zone boots
when force-enabled in a dev config; octree unit tests (port `SpaceZoneTest.cpp`) pass.
**Effort:** 2-4 iteration-days.

---

## Phase 3 — Space Subsystem Transplant (C++)

**Goal:** all clustered space C++ in-tree and compiling. Mostly additive — the clusters have no
Stardust collisions except where they touch shared managers.

| # | Work item |
|---|---|
| 3.1 | Delete the 7-file vanilla ship stub; transplant `objects/ship/**` wholesale (40 + ai 52 + components 35 + events/squadron/transform 21) |
| 3.2 | Transplant managers: `managers/ship/` (14), `managers/space/` (4), `managers/spacecollision/` (3), `managers/spacecombat/` (7) |
| 3.3 | Transplant packets: `packets/jtl/` (5), `packets/ship/` (16), space items in `packets/object/` (10) |
| 3.4 | Transplant commands (16) + register in `CommandConfigManager` |
| 3.5 | Transplant space areas/regions/objects: `objects/{area,region}/space/`, `SpaceObject.idl`, `ShipControlDevice.idl`, deeds, terminals, repair kits, paint kits |
| 3.6 | Transplant `src/templates/tangible/ship/`, `ShipFlag.h`, `SpaceTerrainAppearance.{cpp,h}` |
| 3.7 | Wire shared touchpoints (the ~600 lines — full table in 03-collision-map.md): DirectorManager (174 ln, heaviest — Lua bindings), PlayerManagerImplementation (68), ObjectManager (48), CreatureObjectImplementation (44), IntangibleObject/Lua plumbing |
| 3.8 | `PlayerSpaceZoneComponent`, `SpaceLaunchPoint`, JTL sui callbacks, `SendJtlRecruitment` |
| 3.9 | Port tests: `SpaceZoneTest.cpp`, `LuaShipAgentTest.cpp` |

**Gate G3:** full build; ground regression clean; dev server with one space zone force-enabled
boots, accepts `LaunchIntoSpace`, ship flies, octree range queries return sane results.
**Effort:** 2-4 iteration-days (high file count but additive; agent-parallel by cluster).

---

## Phase 4 — Content Drop (Lua + config)

**Goal:** all ~6,400 space Lua files in-tree, reconciled with Stardust custom_scripts.

| # | Work item |
|---|---|
| 4.1 | Transplant data-template bulk (mostly generated; copy + spot-verify): `object/ship/**`, `object/tangible/ship/**`, `object/{static,tangible}/space/**`, `object/draft_schematic/space/`, `loot/{groups,items}/{ship,space}/` (~5,500 files) |
| 4.2 | Transplant logic set (~150 files): `managers/space/` + regions, `ai_space/`, `screenplays/space/**` (squadrons, missions, spawners, stations), `ship_mobile/**` (1,925 loadouts + patrols), `mobile/space/**` + conversations, command lua |
| 4.3 | Reconcile `serverobjects.lua` / object registration with Stardust custom_scripts includes |
| 4.4 | config.lua: add `SpaceZonesEnabled` 10-zone list; `Core3.JTL.JTLEnabled = false` (dark launch); confirm `dofile` ordering with custom_scripts |
| 4.5 | Loot-table reconciliation: space loot groups vs Stardust custom loot economy (infinity/custom groups) — flag conflicts, do not auto-merge balance |
| 4.6 | **Client asset check:** verify the MTG + stardust TRE stack coexists with JTL client assets (JTL ships in the base client post-2004; risk = MTG patch TREs overriding space .iff/datatables). Test with the actual Stardust client manifest |

**Gate G4:** server boots with all 10 space zones enabled in dev; each zone loads regions,
spawners run, NPC ships spawn; no Lua errors in log sweep.
**Effort:** 1-3 iteration-days.

---

## Phase 5 — Stardust Reconciliation (the customization merge)

**Goal:** space systems work WITH Stardust's combat identity, not against it.

| # | Work item | Why |
|---|---|---|
| 5.1 | **Mind-pool removal vs space combat:** audit every pilot/space read of HAM attributes (ShipObjectImplementation, SpaceCombatManager, pilot command costs) against Stardust's 2-pool model | Highest-probability runtime breakage; upstream assumes 3 pools |
| 5.2 | Pilot profession trees vs Stardust skill changes: wire the 3 faction pilot trees + ace progression into Stardust's modified SkillManager; confirm certification gating fires | skill_manager exists upstream; Stardust changed profession structure (Armorweaver etc.) |
| 5.3 | Squadron themepark XP/credit rewards vs Stardust economy rebalance | Mission rewards calibrated to vanilla economy |
| 5.4 | DirectorManager merge: Stardust screenplay hooks + 174 lines of upstream space bindings | Both sides modified the same file |
| 5.5 | Badge-Jedi / Force-sensitivity interactions with JTL badges (JTL grants badges; Stardust uses badges as Jedi triggers) | Avoid accidental unlock-path changes |
| 5.6 | DB migration: new object types (ships, components, control devices) into the Berkeley DB object hierarchy; verify existing player data unaffected | Schema snapshot from 0.4 is the diff base |

**Gate G5:** ground regression clean; a test pilot levels through tier 1 of one squadron chain
on a Stardust-configured server; existing character data loads clean.
**Effort:** 2-4 iteration-days. 5.1 and 5.4 are the careful ones.

---

## Phase 6 — Verification, Tuning & Launch (the upstream 10-20%, plus ours)

This is where the port inherits upstream's *actual* remaining work (see 04-verification.md for
the full instrumentation/QA/balance program). Summary:

| Track | Contents | Source |
|---|---|---|
| 6.A Instrumentation | Headless-client behavioral harness; pass/fail map of every subsystem; turns "we think it works" into a defect list | 04 §A |
| 6.B Fidelity | Component stat curves + flight constants from client datatables (clean-room, data-only); the interceptor death-spiral fix | 04 §B |
| 6.C Balance sim | Monte Carlo ship/component permutations; auto-tune before a human flies | 04 §C |
| 6.D Multi-crew POB | Station sync correctness (pilot + gunners + interior occupancy) — genuine engineering, least AI-acceleratable | 04 §D |
| 6.E Content QA | Walk all 9 squadron chains + 18 mission types end-to-end; fill conversation/reward gaps | 04 §E |
| 6.F Perf & seams | Octree/broadcast tuning under dogfight load; every spaceport launch/land; hyperspace routing incl. whether the 6 custom planets get space sectors | 04 §F |

**Gate G6 (LAUNCH):** `Core3.JTL.JTLEnabled = true` on live, staged zone-by-zone if desired.
**Effort:** dominated by 6.D and 6.E; everything else is harness-driven.

---

## Sequencing & parallelism notes

- Phases 0→1→2→3 are strictly sequential (each builds on the last).
- Phase 4 (Lua) can begin in parallel with late Phase 3 — content has no compile dependency.
- Phase 5 items 5.2/5.3/5.5 can run in parallel; 5.1 and 5.4 first.
- The Phase 6 harness (6.A) should be BUILT during Phases 2-3 so it is ready the moment G4 passes.
- Single biggest unknown that swings all Phase 6 estimates: whether upstream squadron chains are
  authored-and-working vs authored-but-untested (05 §open-questions). Code density suggests
  working-but-unverified. Phase 6.A answers it definitively; commit to no effort promises before then.