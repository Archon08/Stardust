#!/bin/bash
# Stardust space-port smoke harness (P0.3 / gate test for every phase)
# Usage: tools/smoke/smoke.sh <path-to-MMOCoreORB-bin-dir> [timeout-seconds]
# Asserts: server boots, all configured zones load, no fatal log errors.
# Exit 0 = smoke pass. Runs in CI or any container with a built core3 binary.

set -u
BIN_DIR="${1:?usage: smoke.sh <MMOCoreORB/bin dir> [timeout]}"
TIMEOUT="${2:-300}"
LOG="$BIN_DIR/log/core3.log"
FAIL=0

note() { echo "[smoke] $*"; }
fail() { echo "[smoke][FAIL] $*"; FAIL=1; }

cd "$BIN_DIR" || { echo "no such dir: $BIN_DIR"; exit 2; }
[ -f "$LOG" ] && mv "$LOG" "$LOG.prev"

note "starting core3 (timeout ${TIMEOUT}s)..."
./core3 &
PID=$!

SECONDS=0
BOOTED=0
while [ $SECONDS -lt "$TIMEOUT" ]; do
  if ! kill -0 $PID 2>/dev/null; then fail "server process exited during boot"; break; fi
  if [ -f "$LOG" ] && grep -qE "initialized|Server started|core3 initialized" "$LOG"; then BOOTED=1; break; fi
  sleep 2
done
[ $BOOTED -eq 1 ] || fail "no boot-complete marker within ${TIMEOUT}s"

if [ $BOOTED -eq 1 ]; then
  # 1. Every zone in conf/config.lua ZonesEnabled must appear as loaded
  ZONES=$(sed -n '/ZonesEnabled[[:space:]]*=/,/}/p' conf/config.lua | grep -oE '"[a-z0-9_]+"' | tr -d '"')
  for z in $ZONES; do
    grep -q "$z" "$LOG" || fail "zone not seen in log: $z"
  done
  note "zone check done ($(echo "$ZONES" | wc -w) zones)"

  # 2. Space zones (only when JTL enabled)
  if grep -qE "^[[:space:]]*JTLEnabled[[:space:]]*=[[:space:]]*true" conf/config.lua 2>/dev/null; then
    SZONES=$(sed -n '/SpaceZonesEnabled[[:space:]]*=/,/}/p' conf/config.lua | grep -oE '"space_[a-z0-9_]+"' | tr -d '"')
    for z in $SZONES; do
      grep -q "$z" "$LOG" || fail "space zone not seen in log: $z"
    done
    note "space zone check done"
  else
    note "JTL disabled - skipping space zone checks"
  fi

  # 3. Fatal error scan
  if grep -niE "FATAL|SIGSEGV|assertion failed|uncaught exception" "$LOG"; then
    fail "fatal markers found in log"
  fi
fi

note "stopping server..."
kill $PID 2>/dev/null; wait $PID 2>/dev/null

if [ $FAIL -eq 0 ]; then note "SMOKE PASS"; exit 0; else note "SMOKE FAIL"; exit 1; fi
