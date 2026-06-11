# P0.2 — Stardust Custom Delta Inventory

Computed 2026-06-11 via GitHub API tree-diff (blob-SHA comparison, no clone).
Base: exact merge-base commit `0dc0bb952575e403ff130da3b2341d4a490c517f` (2018-05-17, "[fixed] Personal tef bug" —
verified true ancestor: compare reports ahead 1546 / behind 0). Head: `unstable` @ `a4e475d` (2019-06-30).
Stardust never deleted an upstream file: delta = 5,782 modified + 14,126 added, 0 removed.

## Headline results for the space port

**GOOD NEWS — the port is materially de-risked vs plan assumptions:**

| Assumption (03-collision-map.md) | Reality |
|---|---|
| DirectorManager.cpp heavily customized (HIGH risk) | **UNMODIFIED** — Stardust extended Lua via `LuaCreatureObject.{cpp,h}` instead. The 174-line upstream space merge is now a clean port |
| Zone/ZoneServer customized for 6 custom planets | **UNMODIFIED** — all custom planets are pure Lua (spawn_manager regions + config.lua). Zone split collides with nothing |
| objects/ship/** possibly touched | **UNTOUCHED** — wholesale replace confirmed safe |
| SkillManager.cpp customized (Armorweaver etc.) | **UNMODIFIED** — profession changes are script-side |
| Mind-pool removal rewrote CreatureObject.idl | **CreatureObject.idl UNMODIFIED**; only `CreatureObjectImplementation.cpp` is modified |

**THE REAL COLLISION SET (C++ files both sides touch):**
1. `managers/player/PlayerManagerImplementation.cpp` — Stardust-modified; upstream adds ~68 space lines
2. `objects/creature/CreatureObjectImplementation.cpp` — Stardust-modified; upstream adds ~44 space lines
3. `objects/scene/SceneObject.idl` + `SceneObjectImplementation.cpp` — Stardust-modified; upstream adds space-awareness; IDL change ⇒ wide regen
4. `managers/combat/CombatManager.cpp` — Stardust combat rebalance; space combat is a separate manager but shared helpers need audit
5. `bin/conf/config.lua` — custom TRE stack + custom planet list vs SpaceZonesEnabled merge

## Scale

| | Modified | Added |
|---|---:|---:|
| Total | 5,782 | 14,126 |
| bin/scripts (.lua, 100%) | 5,671 | 14,124 |
| src/server (C++/IDL) | 107 | 2 |
| bin/conf | 2 | 0 |

- Scripts modified: mobile 2,714 · object 2,339 (draft_schematic 1,438 — near-total crafting rebalance) · loot 289 · commands 240 (combat tuning of every special) · screenplays 68
- Scripts added: object/custom_content 9,616 (post-CU/NGE client template import) · mobile/custom_content 2,415 (incl. ep3 355, Shadows of Mustafar 147) · loot 505 · screenplays 159 · spawn_manager regions for chandrila, florrum, hutta, kaas, lothal, moraband, mustafar
- src added: only 2 files (setNormalTask.h, BountyHuntSuiCallback.h)

## Custom planets — corrected list

- config.lua-enabled custom zones: chandrila, coruscant, jakku, kaas, moraband, hutta
- spawn_manager region content also exists for: florrum, lothal, mustafar
- ALL implemented without Zone C++ changes; they ride the standard zone loader + client TREs

## src/** modified (107 files) — categorized

**Combat (core):** CombatManager.cpp, WeaponObjectImplementation.cpp, ArmorObjectImplementation.cpp, DamageOverTime.cpp, BuffAttribute.h, BuffCRC.h, PerformanceBuffImplementation.cpp, StimPack/RangedStimPackImplementation.cpp, GroupManager.cpp + ~52 command headers (CombatQueueCommand.h; LegShot1-3, Melee1hBodyHit1-3, Melee2hSweep1/2, PolearmLegHit1-3, PolearmStun1/2, StoppingShot, SurpriseShot, Steadyaim, VolleyFire, Boostmorale, Rally, Formup, Retreat, DeathBlow, Duel, HealDamage, QuickHeal, Tend(Damage), Prone, Stand, medic set)

**Jedi/FRS (heaviest custom C++ area):** FrsManager.idl, FrsManagerImplementation.cpp, FrsManagerData.idl, FrsRank.idl, ArenaChallengeData.idl, VoteStatusTask.h, EnclaveContainerComponent.{cpp,h}, EnclaveVotingTerminalSuiCallback.h, EnclaveTerminalMenuComponent.cpp, LightsaberCrystalComponentImplementation.cpp, JediRobeAttributeListComponent.h, Force* command set (Armor1/2, Choke, Feedback1/2, Intimidate1/2, Run1/2/3, Shield1/2, ChannelForce, DrainForce, ForceHealQueueCommand.{cpp,h})

**Player/profession:** PlayerManagerImplementation.cpp, PlayerObjectImplementation.cpp, PlayerCreationManager.cpp, Crafting/Entertaining/Slicing/Survey/InterplanetarySurveyDroid sessions, ResourceManagerImplementation.cpp, ResourceContainer.idl, FactoryCrate.idl, LogoutTask.h

**Scene/core (high blast radius):** SceneObject.idl, SceneObjectImplementation.cpp, CreatureObjectImplementation.cpp, LuaCreatureObject.{cpp,h} (extended Lua API — Stardust's chosen extension surface)

**World/NPC:** AiAgentImplementation.cpp, PetManagerImplementation.cpp, PetControlDevice/VehicleControlDevice/VehicleObjectImplementation.cpp, PetMenuComponent.cpp, PetTrickCommand.h, MissionManagerImplementation.cpp, DestroyMissionLairObserverImplementation.cpp, CityManagerImplementation.cpp, PlanetManagerImplementation.cpp, BuildingObjectImplementation.cpp, SarlaccEruptTask.h, RepairVehicleSuiCallback.h, RotateFurnitureCommand, ChatManager.idl/Implementation.cpp, LootManagerImplementation.cpp, AuctionManager.idl

## Hygiene notes for later phases

- Accidentally committed duplicates exist and will load-order-collide if naively merged:
  `recruiterScreenplay - Copy.lua`, jakku `* - Copy.lua` x2, `themeParkJabba-backup.lua`
- Stardust history contains pre-fork Core3 commits upstream has since rewritten/GC'd — harmless; merge-base is SHA-exact
- No build-system changes anywhere in the delta (CMake/idl untouched) — P1 toolchain work collides with nothing
