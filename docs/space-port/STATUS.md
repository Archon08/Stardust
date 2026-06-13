# Space Port — Status Log

## 2026-06-11 — P1 BUILD GREEN ✅

`space-port/phase-1` compiles ALL 1293 targets and links `core3` against engine3 @ `7012c03`
(clang-17, C++14, debian bookworm container). The engine-uplift risk (R1, the plan's biggest
unknown) is retired. 14 CI iterations, error curve: 292 -> 118 -> 324* -> 108 -> 49 -> 6 -> 2 -> 0
(*more tree reachable).

### What it took (full detail in git log, prefixes P1.x)
- engine3 pinned as submodule at Core3's exact pin `7012c03`; modern CMake scaffolding (C++14, clang>=16)
- NULL -> nullptr sweep: 8,763 substitutions, string/comment-aware (cloud sweep workflow, self-tested)
- .idl null comparisons -> truthiness style (idlc REJECTS `nullptr` identifier; emits ambiguous `NULL`)
- clang-17 toolchain (matches upstream; g++ `__null`-as-long ambiguity)
- engine3 API drift: Time/SerializableTime/AtomicTime split, Serializable overloads, mysql wrapper
  migrated into server/db/mysql (engine removed it), PingClient/SocketAddress, pathfinding
  const-correctness + TriangleNode pure virtuals, queueUpdateToDatabaseTask removal, isDeplyoed->isDeployed,
  Reference<> (not ManagedReference<>) for non-managed droid module data components, libatomic link
- core3client build disabled (ENABLE_BUILD_CLIENT=OFF) — upstream rewrote it; port in Phase 6

### G1 gate remaining
- Ground-regression smoke: boot server in CI (mariadb + Berkeley DB env), run tools/smoke/smoke.sh,
  verify all ground zones load (incl. the 6 custom planets) with zero fatal log markers
- Then Phase 2: the Zone -> GroundZone/SpaceZone split

### CI quirks (for whoever drives this next)
- Workflows must exist on default branch `unstable` to be dispatchable; GITHUB_TOKEN pushes do NOT
  auto-trigger builds — dispatch manually after sweep pushes

## 2026-06-13 — P2+P3 FULLY GREEN: space subsystem integrated ✅✅

Branch `space-port/phase-2` @ `42bf538d`, CI build `27453563194` = **success**:
`[1488/1488] Linking CXX executable src/core3` — 0 compile errors, 0 link errors.

The modern Core3 JTL space subsystem (Zone→Ground/Space split, octree, ship objects,
space managers, AI behavior trees, packets, commands) is compiled and linked INTO the
2019 Stardust tree against engine3 @ 7012c03, with Stardust's customizations preserved.

### How it was done (15 closure iterations, ~20 agent-sessions)
- Transplant: 272 C++ files +45k lines from Core3 pin 6856f315 + dependency closure.
- idlc clean after 6 closure iters; C++ compile: 894→0 errors across ~15 builds.
- Link: ported ~60 idl-declared-but-unimplemented native bodies from upstream; removed
  an accidental duplicate SpaceshipTerminal dir (duplicate symbols).
- Whole strata modernized: zone, scene, variables (Delta*), AI behavior-tree (node/leaf/
  decorator), packets, json-serialization (to_json chain), IDL contracts.
- Deep reconciliations: ThreatMap re-keyed CreatureObject*→TangibleObject* (ships as
  threats — the shared-combat keystone); PlayerManager/ActiveArea-flags/CreatureTemplate.

### Stardust protections held throughout
Entertainer fields kept (performanceCounter/instrumentID/terrainNegotiation); CreatureFlag::
enum preserved (ObjectFlag bit values DIFFER — a data-corruption landmine avoided); all 107
protected files edited additively/surgically; 2 real pre-existing Stardust bugs found+fixed
(brace-less loot-credits if; missing auction filter args).

### Remaining to a runnable space server
- **Phase 4 (next):** ~6,400 space Lua scripts content drop + registration reconciliation
  (objects.lua/screenplays.lua/loot/managers includes), JTL stays dark until verified.
- **Boot gate:** server boots with space zones loading (CI init-check + operator-run full
  smoke with MTG/stardust TREs).
- **Functional punch list:** space-XP impl bodies, auction filter params, bandSong, AiMap
  Stardust counters — all compile-safe deferrals for Phase 5/6.
