# Operator Runbook — Boot the Space Port (P4 → live)

Goal: build `space-port/phase-2`, boot it against real SWG client TREs with JTL enabled,
and capture the boot log. That log is the input that turns "compiles" into a runnable
defect list and unblocks Phase 5/6.

## 0. What you need
- A **pre-CU/JTL-era SWG client** (JTL shipped Oct 2004, so the base client already contains
  all space `.iff` + datatable assets — no separate "space TRE" needed).
- The **MTG (ModTheGalaxy) patch TREs** + **Stardust TREs** that `bin/conf/config.lua` `TreFiles`
  already lists (stardust_s.tre, stardust_01-03.tre, mtg_patch_*.tre, mtg_planets.tre).
- Linux box or WSL2 (the build is debian-bookworm + clang-17 based).

## 1. Build (authoritative recipe = the CI workflow)
The canonical, known-green build is `.github/workflows/space-port-build.yml`. Mirror it locally:
```bash
git clone -b space-port/phase-2 --recurse-submodules https://github.com/Archon08/Stardust
cd Stardust
# deps (debian/ubuntu): clang-17, cmake, ninja, default-jre-headless, liblua5.3-dev,
#   libdb5.3-dev, libmariadb-dev, libssl-dev, zlib1g-dev, libboost-thread/system-dev, libjemalloc-dev
cd MMOCoreORB
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCOMPILE_TESTS=OFF \
      -DENABLE_NATIVE=OFF -DENABLE_BUILD_CLIENT=OFF
ninja -C build          # produces src/core3  (15 closure rounds say this links clean)
```
engine3 is pinned as a submodule at `7012c03`; `--recurse-submodules` is required.

## 2. Database
MariaDB/MySQL with the Stardust schema:
```bash
mysql -e "CREATE DATABASE swgemu; CREATE USER 'swgemu'@'localhost' IDENTIFIED BY '123456';
          GRANT ALL ON *.* TO 'swgemu'@'localhost'; FLUSH PRIVILEGES;"
mysql swgemu < MMOCoreORB/sql/swgemu.sql
mysql swgemu < MMOCoreORB/sql/datatables.sql
mysql swgemu < MMOCoreORB/sql/mantis.sql
```
(config.lua already points at 127.0.0.1:3306 swgemu/swgemu/123456 — adjust if yours differs.)

## 3. Config for space
In `MMOCoreORB/bin/conf/config.lua`:
- `TrePath` → your client TRE directory (where the .tre files live).
- Confirm `TreFiles` matches what you have (base SWG client TREs + MTG + stardust — already listed).
- **Enable JTL:** set `Core3.JTL.JTLEnabled = true` (it ships dark/false). `SpaceZonesEnabled`
  (10 zones) is already present from P4.

## 4. Boot + capture
```bash
cd MMOCoreORB/bin
./core3 2>&1 | tee boot.log
# or, for the structured gate:
../../tools/smoke/smoke.sh MMOCoreORB/bin 600
```
Let it run until it either finishes zone init or errors out.

## 5. What to send back
- The full `boot.log` (or core3.log under bin/log/).
- Especially: which `space_*` zones report loaded, any Lua errors during space screenplay /
  ship_mobile / object-template load, and whether `LaunchIntoSpace` / pilot flow is reachable.

## What this unblocks
- **P5** — wire pilot cert trees + squadron rewards into Stardust's skill/economy (now verifiable).
- **P6** — instrumentation/QA/balance: walk the 9 squadron chains, the interceptor death-spiral,
  fidelity tuning. This is where space becomes *playable*, not just compiled.

## Current branch state (resume pointer)
- `space-port/phase-2` @ HEAD — P2+P3 compile+link green (tag `p2p3-green`), P4 content validated
  (space-lint: syntax + byte-fidelity-vs-upstream + registration all PASS; g1-smoke boots to TRE).
- Workflows available: space-port-build, g1-smoke, space-lint, mechanical-sweep, space-import,
  space-content (all dispatchable; also on default branch).
