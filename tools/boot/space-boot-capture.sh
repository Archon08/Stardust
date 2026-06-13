#!/bin/bash
###############################################################################
# Stardust SPACE boot-capture harness  (F1)
#
# For a helper who is NOT the developer: run this ONE script. It boots the
# server with JTL/space enabled, captures EVERYTHING needed to debug remotely
# (boot log, errors, Lua errors, missing assets, crash backtrace, zone-load
# status, DB + environment), and packages it into ONE file to send back.
#
# USAGE:
#   ./space-boot-capture.sh /path/to/MMOCoreORB /path/to/TRE_directory
#   (or set MMO_DIR and TRE_DIR env vars and run with no args)
#
# OUTPUT:
#   space-boot-report-<timestamp>.tar.gz   <- send this whole file back
#   (and SUMMARY.txt is printed to screen at the end)
###############################################################################
set -u

MMO_DIR="${1:-${MMO_DIR:-}}"
TRE_DIR="${2:-${TRE_DIR:-}}"
BOOT_SECONDS="${BOOT_SECONDS:-240}"     # how long to let it run before stopping
TS="$(date +%Y%m%d-%H%M%S 2>/dev/null || echo run)"
WORK="$(pwd)/space-boot-report-$TS"
mkdir -p "$WORK"
SUM="$WORK/SUMMARY.txt"

say(){ echo "$*" | tee -a "$SUM"; }
hr(){ echo "------------------------------------------------------------" | tee -a "$SUM"; }

say "Stardust space boot-capture  ($TS)"
hr

############ 0. locate things, friendly errors ############
if [ -z "$MMO_DIR" ] || [ ! -d "$MMO_DIR" ]; then
  echo "ERROR: pass the MMOCoreORB directory as arg 1 (or set MMO_DIR)."
  echo "   example: ./space-boot-capture.sh ~/Stardust/MMOCoreORB ~/SWG_TREs"
  exit 2
fi
BIN="$MMO_DIR/bin"
CORE3=""
for c in "$BIN/core3" "$MMO_DIR/build/src/core3"; do [ -x "$c" ] && CORE3="$c" && break; done
if [ -z "$CORE3" ]; then
  say "ERROR: core3 binary not found (looked in bin/ and build/src/). Build it first."
  say "       (build recipe = .github/workflows/space-port-build.yml)"
fi
say "MMOCoreORB : $MMO_DIR"
say "core3 bin  : ${CORE3:-NOT FOUND}"
say "TRE dir    : ${TRE_DIR:-<not provided>}"

############ 1. environment snapshot ############
{
  echo "=== uname ==="; uname -a 2>&1
  echo "=== os-release ==="; cat /etc/os-release 2>/dev/null | head -3
  echo "=== gdb? ==="; command -v gdb || echo "no gdb (backtrace on crash will be limited)"
  echo "=== mysql/mariadb? ==="; command -v mysqld mariadbd mysql 2>/dev/null
  echo "=== lua ==="; command -v lua5.3 lua 2>/dev/null
  echo "=== free mem ==="; free -h 2>/dev/null | head -2
} > "$WORK/environment.txt" 2>&1
say "environment -> environment.txt"

############ 2. config snapshot + JTL/TRE check ############
CFG="$BIN/conf/config.lua"
if [ -f "$CFG" ]; then
  cp "$CFG" "$WORK/config.lua.used" 2>/dev/null
  JTL=$(grep -iE "JTLEnabled" "$CFG" | head -1)
  say "config.lua JTL line: ${JTL:-<none found>}"
  if ! grep -qiE "^[[:space:]]*JTLEnabled[[:space:]]*=[[:space:]]*true" "$CFG"; then
    say "WARNING: JTLEnabled is not =true in config.lua -> space zones will NOT load."
    say "         (edit conf/config.lua: set  Core3.JTL.JTLEnabled = true  OR  JTLEnabled = true)"
  fi
  grep -nE "TrePath|SpaceZonesEnabled" "$CFG" >> "$WORK/config_keys.txt" 2>/dev/null
else
  say "WARNING: $CFG not found."
fi
# TRE presence check (the space data lives in mtg_patch_013_configurable_02.tre)
if [ -n "${TRE_DIR:-}" ] && [ -d "$TRE_DIR" ]; then
  ls -la "$TRE_DIR" > "$WORK/tre_listing.txt" 2>&1
  for need in mtg_patch_013_configurable_02.tre stardust_01.tre; do
    [ -f "$TRE_DIR/$need" ] && say "TRE ok: $need" || say "TRE MISSING: $need (space data needs it)"
  done
fi

############ 3. database up + schema ############
DBLOG="$WORK/db_setup.txt"
{
  if command -v mysqladmin >/dev/null 2>&1 && mysqladmin ping >/dev/null 2>&1; then
    echo "mysql already running"
  elif command -v mysqld_safe >/dev/null 2>&1; then
    echo "starting mariadb..."; (mysqld_safe >/dev/null 2>&1 &) ; sleep 8
  fi
  if command -v mysql >/dev/null 2>&1; then
    mysql -e "CREATE DATABASE IF NOT EXISTS swgemu; CREATE USER IF NOT EXISTS 'swgemu'@'localhost' IDENTIFIED BY '123456'; CREATE USER IF NOT EXISTS 'swgemu'@'127.0.0.1' IDENTIFIED BY '123456'; GRANT ALL ON *.* TO 'swgemu'@'localhost'; GRANT ALL ON *.* TO 'swgemu'@'127.0.0.1'; FLUSH PRIVILEGES;" 2>&1
    for s in swgemu datatables mantis; do [ -f "$MMO_DIR/sql/$s.sql" ] && mysql swgemu < "$MMO_DIR/sql/$s.sql" 2>&1 && echo "loaded sql/$s.sql"; done
    echo "=== tables ==="; mysql swgemu -e "SHOW TABLES;" 2>&1 | head
  else
    echo "NO mysql client - DB not set up (server will fail at DB connect)"
  fi
} > "$DBLOG" 2>&1
say "database setup -> db_setup.txt"

############ 4. BOOT under gdb (backtrace on crash) ############
if [ -n "$CORE3" ]; then
  cd "$BIN" || exit 2
  mkdir -p log databases navmeshes
  BOOTLOG="$WORK/boot-stdout.log"
  say "booting core3 for up to ${BOOT_SECONDS}s (capturing all output + crash backtrace)..."
  if command -v gdb >/dev/null 2>&1; then
    timeout "$BOOT_SECONDS" gdb -batch -ex run -ex "thread apply all bt" -ex quit --args "$CORE3" > "$BOOTLOG" 2>&1
  else
    timeout "$BOOT_SECONDS" "$CORE3" > "$BOOTLOG" 2>&1
  fi
  EXIT=$?
  say "core3 stopped (exit/timeout code $EXIT)"
  # collect server logs
  cp -r "$BIN/log" "$WORK/server_log" 2>/dev/null
fi

############ 5. ANALYSIS / DIGEST (the stuff the dev reads first) ############
ALL="$WORK/boot-stdout.log"
DIG="$WORK/ERROR_DIGEST.txt"
if [ -f "$ALL" ]; then
  {
    echo "=== boot depth (last 5 progress lines) ==="; tail -5 "$ALL"
    echo; echo "=== FATAL / crash markers ==="; grep -inE "FATAL|SIGSEGV|segmentation|terminate called|assertion|abort|dumped core|#[0-9]+ +0x" "$ALL" | head -40
    echo; echo "=== Lua errors ==="; grep -inE "lua|attempt to|stack traceback|\.lua:[0-9]" "$ALL" | head -40
    echo; echo "=== missing templates / files ==="; grep -inE "could not (find|open|load)|does not exist|not found|missing|unknown template" "$ALL" | head -40
    echo; echo "=== generic ERRORs ==="; grep -inE "] ERROR" "$ALL" | head -50
    echo; echo "=== SPACE ZONE load status (10 expected) ==="
    for z in space_corellia space_dantooine space_dathomir space_endor space_heavy1 space_light1 space_lok space_naboo space_tatooine space_yavin4; do
      if grep -qi "$z" "$ALL"; then echo "  LOADED?  $z   (seen in log)"; else echo "  NOT SEEN $z"; fi
    done
    echo; echo "=== ship spawn / SpaceManager evidence ==="; grep -inE "SpaceZone|ShipAgent|SpaceManager|spawn.*ship|ship.*spawn" "$ALL" | head -20
  } > "$DIG" 2>&1
  say ""; say "=== TOP-LINE VERDICT ==="
  grep -qiE "DistributedObjectBroker] started" "$ALL" && say "  ORB: started" || say "  ORB: NOT started"
  ZN=$(for z in corellia dantooine dathomir endor heavy1 light1 lok naboo tatooine yavin4; do grep -qi "space_$z" "$ALL" && echo x; done | wc -l)
  say "  space zones seen in log: $ZN / 10"
  say "  FATAL/crash markers: $(grep -icE 'FATAL|SIGSEGV|segmentation|dumped core' "$ALL")"
  say "  Lua errors:          $(grep -icE 'attempt to|stack traceback|\.lua:[0-9]' "$ALL")"
  say "  missing-asset errors: $(grep -icE 'does not exist|not found|could not (find|open|load)' "$ALL")"
  say "  digest -> ERROR_DIGEST.txt"
fi

############ 6. bundle ############
cd "$(dirname "$WORK")"
BASE="$(basename "$WORK")"
if command -v tar >/dev/null 2>&1; then
  tar -czf "$BASE.tar.gz" "$BASE" 2>/dev/null && say "" && say "BUNDLE READY: $(pwd)/$BASE.tar.gz  <-- send this file back"
else
  say "tar not available; send the whole folder: $WORK"
fi
hr
say "Done. Paste SUMMARY.txt below if you can't send the file."
cat "$SUM"
