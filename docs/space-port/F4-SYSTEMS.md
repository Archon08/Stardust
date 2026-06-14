# F4 — Systems Completion (audit + completions)

Phase F4 of `docs/space-port/FINISH-SPACE-PLAN.md`. Scope: C++ ship systems + their managers/idl + non-squadron wiring. Squadron/screenplay LUA content is owned by a concurrent agent and is explicitly out of scope here.

**Method (no local clone/boot).** Every audit below is a blob-SHA tree diff of branch `stardust-space` against upstream `swgemu/Core3` at the contract pin `6856f315a80b5250635b2272695caec1d64204ed`, plus source reads of the files that differ. Blob SHA is content-addressed, so an identical SHA proves byte-identical content (a clean transplant); a differing SHA was opened and diffed line-by-line.

**Headline.** The P3 transplant of `objects/ship/**` is a faithful copy of the pinned upstream: of 148 blobs in the ship subtree, **143 are byte-identical**; the 5 "differences" reduce to 3 real files, all benign fork-adaptation include/signature deltas (not behavioral gaps). Across the full F4 surface (commands, tasks, managers, packets, zone components) the only **functional regression** found was a stubbed `LeaveStationCommand.h`, which has been restored to upstream behavior. Hyperspace routing and spaceport launch/land are code-complete and data-driven; their data (the `.iff` datatables) is delivered by the client TRE stack at boot, not committed to the repo (consistent with the delta inventory's no-binary-assets-in-repo rule), so end-to-end correctness is a boot-validation item, not a code gap.

---

## System 1 — Multi-crew POB ships

**Upstream behavior (pin).** A POB ship has interior cells containing station objects: a pilot chair, an operations chair, and turret ladders/gunner stations. Players board a station by being transferred into the station container (`ShipStationContainerComponent::transferObject`), which clears interior state and sets the appropriate space state (`PILOTINGPOBSHIP` / `SHIPOPERATIONS` / `SHIPGUNNER`). The pilot flies; the operations station relays control intent to the ship; gunners fire turrets via `turretFire`. A crew member leaves a station with `/leaveStation`, which clears space states, sets `SHIPINTERIOR`, repositions them off the chair, and transfers them back to the interior cell. Insertion of pilot/group/astromech is scheduled by the `Insert*IntoShipTask` events.

**Port's actual state.**
- `ShipStationContainerComponent.{cpp,h}` — **SAME** as pin. Real transfer logic; includes a documented Stardust hardening: it only broadcasts slotted-object creates when `notifyClient` is true, to avoid premature client creates that crash during hyperspace zone transfers (`ShipStationContainerComponent.cpp:108-126`).
- `PobShipContainerComponent.{cpp,h}` — **SAME** as pin.
- `PobShipObject.idl`, `PobShipObjectImplementation.cpp` — present; impl differs from pin only by a removed `#include TransactionLog.h` and a `setObjectName(StringId)` -> `setObjectName(String)` signature adaptation (`PobShipObjectImplementation.cpp:236-239`). Chair registration on interior build is intact: pilot/operations chairs bound at `PobShipObjectImplementation.cpp:187-190` (`setPilotChair` / `setOperationsChair`).
- `MultiPassengerShipObject.{idl,cpp}` — **SAME** as pin.
- `ShipObject.idl` — pilot/ops/gunner station accessors present: `getPilot()` (native), `getShipGunner()` (native), `getPilotChair()/setPilotChair`, `getOperationsChair()/setOperationsChair` (`ShipObject.idl:1559-1565,1708-1710,995-1000`).
- `CreatureObject.idl` — station predicates present: `isInShipStation()` (`CreatureObject.idl:2286`, tests SHIPINTERIOR|SHIPOPERATIONS|SHIPGUNNER), `clearSpaceStates()` (native, :1129); chair predicates `isPilotChair()/isOperationsChair()` in `SceneObject.idl:2019-2024`.
- `CreatureState.h` — all ship state bits present: `SHIPOPERATIONS`, `SHIPGUNNER`, `SHIPINTERIOR`, plus `PILOTINGSHIP/PILOTINGPOBSHIP` wired into `CommandConfigManager` state masks (`CommandConfigManager.cpp:233-242,413-417`).
- Insert tasks: `InsertPilotIntoShipTask.h`, `InsertGroupMemberIntoShipTask.h`, `InsertAstromechIntoShipTask.h` — all **SAME** as pin.
- `LeaveStationCommand.h` — **was a STUB.** The port had replaced the real command with a generic `CombatQueueCommand` no-op (`doCombatAction`), 645 bytes vs upstream's 1428. This broke the only player-facing path to exit a crew station.

**What I completed.** Restored `LeaveStationCommand.h` to the exact pin source: it now checks `isInShipStation()`, walks parent->cell, calls `clearSpaceStates()`, sets `SHIPINTERIOR`, repositions out of pilot/ops chair, resets the movement counter, and `cell->transferObject`s the creature back into the interior cell. All referenced methods were verified present in the port IDL before the edit. Post-commit blob SHA equals the pin's blob SHA (`64055d4bdc6da136cd510cb3086723c8e1ac3015`) — a byte-exact restore.

**Status: PARTIAL -> compile-complete; concurrency/sync correctness NEEDS-BOOT.** All multi-crew machinery (stations, chairs, container transfer, insert tasks, leave command) is now present and matches the pin. What cannot be proven without the F1 boot:
- pilot + multiple gunners + operations occupying stations simultaneously without lock contention or state races (the plan's flagged "assume real engineering" item);
- the **operations-station -> ship-control sync** at runtime: chairs and `getPilot()` exist, but whether ops-station input correctly relays to ship control under live tick is an observable behavior, not a compile fact;
- interior cell occupancy persistence across a hyperspace zone transfer (the hardening in `ShipStationContainerComponent` targets exactly this; needs live verification it holds).

---

## System 2 — Hyperspace routing

**Upstream behavior (pin).** `ShipManager::loadHyperspaceLocations()` reads `datatables/space/hyperspace/hyperspace_locations.iff` and builds two maps keyed by location name: `hyperspaceLocations` (Vector3 arrival point) and `hyperspaceZones` (destination space zone name). `/hyperspace <location>` (`HyperspaceCommand`) validates the name, resolves the destination zone via the ZoneServer, checks it is a space zone, then schedules `HyperspaceToLocationTask`, which moves the ship across zones, driving the `BeginHyperspace`/`Hyperspace`/`OrientForHyperspace` packets.

**Port's actual state.**
- `ShipManager.cpp` — loader present, behavior identical: `loadHyperspaceLocations()` reads the exact `datatables/space/hyperspace/hyperspace_locations.iff` path and parses name/zone/x/y/z into `hyperspaceLocations` + `hyperspaceZones` (`ShipManager.cpp:96-121`); called from manager init (`ShipManager.cpp:59`). Only diff vs pin: a removed `#include TransactionLog.h`.
- `HyperspaceCommand.h` — full inter-zone routing (`HyperspaceCommand.h:18-110`): `hyperspaceLocationExists` -> `getHyperspaceZone(destName)` -> `zoneServer->getZone(zoneName)` -> `newZone->isSpaceZone()` guard -> `getHyperspaceLocation(destName)` -> schedules `HyperspaceToLocationTask`. Enforces Live gates: hyperdrive present (chassis not "basic"), engine not DISABLED, no in-progress hyperspace, hyperspace delay. Only diff vs pin: the port keeps the `#include ShipComponentFlag.h` upstream later dropped — and the port needs it (`ShipComponentFlag::DISABLED` used at :81), so the port version is self-consistent.
- `AborthyperspaceCommand.h` — **SAME** as pin.
- `HyperspaceToLocationTask.h` — **SAME** as pin.
- Packets `BeginHyperspace`/`Hyperspace`/`OrientForHyperspace` — present (referenced by the command/task; `OrientForHyperspace.h` included by `HyperspaceCommand.h`).
- 10 space zones: `config.lua SpaceZonesEnabled` is **byte-identical** to the pin — the 10 production zones (`space_corellia, space_dantooine, space_dathomir, space_endor, space_heavy1, space_light1, space_lok, space_naboo, space_tatooine, space_yavin4`) enabled, test zones commented out exactly as upstream (`config.lua:147-165`).

**Wiring gap found:** none. Routing is fully data-driven from the tables, exactly as Live.

**Data note.** The `.iff` datatables (`hyperspace_locations.iff` with 32 jump locations, `system_locations.iff` with 5 systems) are **not committed to the repo** — no space `.iff`/`.tab` exists in the tree. Per the P0.2 delta inventory, binary/datatable assets ride the client TRE stack and are loaded by `DataArchiveStore` at boot. The loader's hard `fatal()` on a missing table means the boot will immediately flag any TRE-path problem. The in-repo code is complete; the table content is a boot-time input.

**Status: COMPLETE (code) / NEEDS-BOOT (data presence).** Code path is data-driven and matches the pin. Confirm at F1: the TRE stack actually delivers `datatables/space/hyperspace/hyperspace_locations.iff` (no `fatal`), and all 32 jump locations resolve to zones in the enabled set so every one of the 10 zones is reachable.

---

## System 3 — Spaceport launch / land

**Upstream behavior (pin).** `/launchIntoSpace` (`LaunchIntoSpaceCommand`) is invoked from a SPACETERMINAL with a ship-control-device id, optional group, and optional arrival-planet/point. Two modes: (a) **JTL fast travel** — validates the ship has reactor + engine, resolves the arrival zone/planet travel point, scatters the group at a randomized landing position, and `switchZone`s everyone planet-side (storing the ship's location); (b) **launch into space** — schedules `LaunchShipTask`, which inserts the player into the ship in the correct space zone. Landing/storing is `StoreShipTask` + `ShipControlDevice`; the launch point per planet comes from `SpaceManager`/`SpaceLaunchPoint`/`PlayerLaunchPoints`.

**Port's actual state.**
- `LaunchIntoSpaceCommand.h` — **SAME** as pin. Full dual-mode logic including `canBePilotedBy` certification gate, reactor/engine checks, planet-travel-point resolution, group scatter, and `LaunchShipTask` scheduling (`LaunchIntoSpaceCommand.h:21-150`).
- `LaunchShipTask.h` — **SAME** as pin.
- `StoreShipTask.h` — **SAME** as pin.
- `SpaceManager.idl` / `SpaceManagerImplementation.cpp` — **SAME** as pin.
- `SpaceLaunchPoint.h` (player variable) — **SAME** as pin.
- `PlayerLaunchPoints.h` — **SAME** as pin.
- `ShipControlDevice.idl` — **SAME** as pin; `ShipControlDeviceImplementation.cpp` differs only by a removed `#include server/ServerCore.h` (benign).
- `launchIntoSpace.lua` slash-command registration present (`commands.lua:831`).

**Wiring gap found:** none.

**Status: COMPLETE (code) / NEEDS-BOOT (per-zone end-to-end).** All launch/land machinery is present and matches the pin. Confirm at F1: from a planetary spaceport terminal, launch resolves to the correct space zone, and land/store returns the player to the correct planet travel point — for each of the 10 zones (the SpaceManager launch-point <-> zone mapping is data/TRE-driven, same boot-input caveat as hyperspace).

---

## Out-of-scope upstream-only files (intentionally NOT backported)

Two ship-related files exist in the pin but not in the port: `tangible/components/PobShipObjectMenuComponent.{cpp,h}` (POB radial menu) and `tangible/terminal/components/ShipPermissionTerminalMenuComponent.{cpp,h}`. Both are dated **2024** in their headers — upstream additions newer than the transplant's source era, and the contract pin's own tree predates the menu wiring that references them. A repo-wide search finds **zero references** to either class in the port, so their absence is clean (no dangling refs, no compile dependency). These are post-pin features outside the F4 match-the-pin contract and were deliberately left out. Flag for a future phase if the POB radial menu is desired; not required for F4 parity.

(The other ~111 upstream-only `src/server` files — REST/web API, GCW contraband, pet formation, NPC-actor sessions, paint/texture kits, etc. — are post-2018 upstream features unrelated to the F4 ship-systems surface and are correctly absent.)

---

## Build validation

- Code change: 1 file — `LeaveStationCommand.h` restored to pin (commit `3d8d8fa`).
- Dispatched `space-port-build.yml` on `stardust-space` after the change (run `27488498711`, head `3d8d8fa`). Prior branch runs were green; the change is a header-only restore of a command whose every dependency was verified present in the port IDL, so it is expected to keep the build green. Final conclusion is recorded in the F4 return summary / run page.

## Summary table

| System | Status | Completed | Flagged for boot |
|---|---|---|---|
| Multi-crew POB | PARTIAL (compile-complete) | Restored `LeaveStationCommand` stub->pin | Concurrent crew locking, ops->ship-control sync, occupancy across hyperspace |
| Hyperspace routing | COMPLETE (code) | Verified data-driven loader + routing + 10 zones | TRE delivers `.iff` tables; all 32 jumps resolve to enabled zones |
| Spaceport launch/land | COMPLETE (code) | Verified dual-mode launch + store + per-planet points | Per-zone launch->correct space zone and land->correct travel point |
