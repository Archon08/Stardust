# Booting Stardust Space — for the helper

You don't need to know the codebase. You run **one script**, it captures everything, and you send
back **one file**. ~10 minutes plus a ~4-minute boot.

## What you need
- A Linux box or Windows WSL (Ubuntu).
- A **built** `core3` server (the `MMOCoreORB` folder, with `bin/core3` compiled).
- A **JTL-era SWG client TRE set** in one folder — must include `mtg_patch_013_configurable_02.tre`
  and the `stardust_*.tre` / `mtg_*.tre` overlay (that's where the space data lives), alongside the
  base client TREs.
- MariaDB/MySQL installed (the script will start it and load the schema).

## Run it
```bash
# from anywhere with write access:
bash tools/boot/space-boot-capture.sh  /path/to/MMOCoreORB  /path/to/your/TRE_folder
```
That's it. The script will:
1. check the binary, config, and TREs,
2. start the database and load the schema,
3. set space on and boot the server for ~4 minutes capturing all output,
4. if it crashes, capture a full backtrace (install `gdb` first for best results: `sudo apt install gdb`),
5. analyze the log (zone-load status, Lua errors, missing assets, crashes),
6. package everything into `space-boot-report-<timestamp>.tar.gz`.

## Send back
- The file **`space-boot-report-<timestamp>.tar.gz`** (preferred — has full logs + backtrace), **or**
- if you can't send a file, copy-paste the **`SUMMARY.txt`** it prints at the end.

## Notes
- It's safe + read-mostly: it only writes logs, a local DB, and the report folder. It does not modify
  the game data or the repo.
- If `JTLEnabled` isn't on, the script warns you and tells you the one line to edit in
  `conf/config.lua` (`Core3.JTL.JTLEnabled = true`).
- A first boot that stops at the "TRE/asset" stage or after zone load is still useful — send the
  report regardless of whether it looks "successful."
