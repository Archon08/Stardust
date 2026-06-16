# Stardust Google Drive — Dev-Environment Inventory & Fetch Guide

Source: shared Google Drive **"Stardust 2.0 Dev Files"** (root folder id `1ZfFoj8giCP2dmdrdZMYDxcKa3wfe7O1Y`)
— 138 folders / 1473 files. Full file/id listing cached locally at `C:\swg-extract\_drive_full.txt`.

**Fetch method (verified working 2026-06-16):** files are shared "anyone with link", so direct download by id:
```
curl -sL "https://drive.usercontent.google.com/download?id=<FILE_ID>&export=download&confirm=t" -o <out>
```
(Small files return immediately; multi-GB files also work but take time. No auth/rclone/gdown needed.)

## What's in the Drive (the Stardust dev environment)

| Item | Path | File ID | Notes |
|------|------|---------|-------|
| **WSL dev-env image** | `WSL Backups/Ubuntu-20_04.tar` | `11waYR5phX35oNFrEResFJ6_T9Nbal4VX` | **Complete Stardust build/dev environment** (Ubuntu 20.04 WSL distro). Multi-GB. `wsl --import` to restore. |
| Engine source | `SWG Stardust Files/PublicEngine-master/` | (folder) `10f8W5XPvDXHcPgZQ3sc0EUD-pN5-VfD5` | Stardust MMOEngine source + `INSTALL` + `README.md`. |
| **Last-Live DB** | `.../Last Live Stardust 1 SQL Dump/Dump20190714.sql` | `1AJoQU6apkhoh58WTL_Lt8jKwZx5tHsn3` | ✅ GRABBED → `C:\swg-extract\stardust-live-db\`. 23.5MB, 56 tables (accounts/characters/galaxy/badges/mantis), 2019-07-14. |
| Server TRE set | `.../MTG Consolidated Server Tre Files/` | (folder) `1u020fKdxTrAOTeXo8od6Q3FCK9CzJlk_` | mtg_patch_001..019,021,022,**023** + planets. **023 was the only one missing locally** → ✅ GRABBED. |
| Stardust TREs | `.../Stardust Tre Files/` | (folder) | stardust_01/02/03. |
| Stardust installer | `.../StardustInstaller.exe` | `1-9oJsnGDi4DN_8C2_OxWbda1M4quM52J` | Windows server installer. |
| Stardust 2 setup | `/Stardust2/Stardust2Setup.exe` | `10hbZKL7lqa6wxxp0QVFdQvbgqJOjFKHi` | |
| **SWGEmu client installer** | `/Stardust2/SWGEmu_Setup.exe` | `1jakMn6YjHfFVS-3PXRhZ5u1m85tb1hXS` | Official SWGEmu client installer → downloads the **complete client TRE set (incl. the NGE ship appearances)**. Multi-GB after install. This is the legit source for the 31 missing assets. |
| Client + launcher | `/Stardust2 Ciiv-Dev/` (SWGEmu.exe, StardustLauncher.exe, TREs) | (folder) | A deployed Stardust client (uses the same MTG TRE repack). |
| IFF/TRE editor | `sytners_iff_editor_3_7_0_95_release/SIE.exe` | `1lVXVBbT1IOWWGHkx14sshy2Ys3yzbtNA` | Sytner's IFF Editor — edit/pack TRE & IFF (useful for authoring ship_chassis/.iff). |

## Grabbed this pass
- `mtg_patch_023.tre` (17.5MB, Kashyyyk+Mustafar snapshots/environment + newer `datatables/player/quests.iff`) → `C:\Stardust-TREs\` (set now **27 TREs**); added to `live.cfg`, `tre_manifest.sha256`, and the boot `config.lua` `TreFiles` (highest mtg priority).
- `Dump20190714.sql` (Last-Live Stardust DB) → `C:\swg-extract\stardust-live-db\`.

## NOT grabbed (large — confirm before pulling)
- **`Ubuntu-20_04.tar`** (the WSL dev environment) — multi-GB; pull only if you want the prebuilt local dev env (we already build via CI + boot via Docker).
- **`SWGEmu_Setup.exe`** + full client install — the route to the **31 missing NGE ship appearances** (`MISSING-ASSETS.md`). Run the installer to get the complete client TRE set, then overlay it **below** the 27 custom TREs.

## Re the missing NGE assets
The Drive's TRE folders contain **no** full-client/NGE TRE set — they're the same MTG/JTL repack (+023). The NGE
ship appearances come only from the **complete SWGEmu client** (via `SWGEmu_Setup.exe`). So the 31-asset gap in
`MISSING-ASSETS.md` is closed by installing that client and adding its base TREs under the overlays — not by any
file already in the Drive.
