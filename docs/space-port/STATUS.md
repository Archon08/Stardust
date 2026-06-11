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
