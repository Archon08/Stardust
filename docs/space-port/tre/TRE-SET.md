# Stardust — Full Client TRE Set (deployment spec)

**Status:** assembled & verified from all sources available on the build machine (2026-06-16).
**Canonical location:** `C:\Stardust-TREs\` (26 TRE files, 5.3 GB) + `live.cfg` (load order).
**This is the complete client asset set available locally.** Integrity: `tre_manifest.sha256`.

---

## 1. What the set is

26 TRE files = a **near-complete JTL-era SWG client repack** (the "MTG" / Modding-the-Galaxy set)
plus Stardust's own custom overlays:

| Layer | Files | Role |
|-------|-------|------|
| Stardust custom | `stardust_s`, `stardust_01/02/03` (4) | Highest priority — Stardust custom content (heroics, custom objects, overrides) |
| MTG client repack | `mtg_patch_001..022` (no 020) + `mtg_planets` (22) | The repacked SWG client (appearance, texture, object, configurable, files, planets) |

**Indexed contents: 203,957 unique assets.** Covers all ground + JTL space content the server loads,
including every ship the JTL space port spawns (X/Y/A/B-wing, TIE variants, gunboats, both Corellian
Corvettes, the SoroSuub yacht) — the headless boot deployed all 10 space zones with **0 spawn errors**.

## 2. Load order (priority: first = highest)

Defined in `live.cfg` (client `searchTree` format) AND mirrored by the server's
`MMOCoreORB/bin/conf/config.lua` `TreFiles` list. Stardust custom first (overrides), then MTG patches
newest→oldest, then planets/textures/appearance. **Do not reorder** — overlays must outrank base content.

## 3. The SWG retail disks are REDUNDANT (do not add them)

The 3 retail install disks (`swg-disk1/2/3.iso`, pre-JTL) extract to 22 base TREs (`bottom.tre`,
`data_*`, `default_patch`). Indexing proves the MTG repack is a **superset**: of 85,153 base assets,
only **1** (`appearance/animation/boss_nass_*.ans`, a Gungan-boss animation) is absent from the overlays.
Adding the 1.8 GB base layer would contribute one unused animation. **The 26 overlays are the set.**

## 4. Coverage gap — 31 assets the repack does NOT contain

The master `datatables/space/ship_chassis.iff` lists every SWG ship chassis (JTL + NGE + special). 294
have appearance `.iff` files in this repack; **24 do not** — so at boot the server logs
`TreeArchive: Did not find fileName: ship_chassis_<x>.iff` for them. These ships simply **cannot spawn**
(no appearance) — **harmless to the JTL space port** (it uses none of them). Plus 7 other absent assets.
Full list + acquisition guidance: `MISSING-ASSETS.md`.

These appearances exist in the **complete retail/NGE SWG client TRE set** (what SWGEmu players install via
the official launcher) — they were simply not included in this MTG/JTL repack subset. To reach 100%
coverage for a future NGE-scope project, place the complete SWGEmu client TRE set in the TrePath **below**
(lower priority than) the Stardust+MTG overlays, then append those base TREs to `config.lua` `TreFiles`.

## 5. Deploying to a live server

1. Put the TrePath dir (these 26 TREs + `live.cfg`) where the server can read it.
2. Set `MMOCoreORB/bin/conf/config.lua` → `TrePath = "<that dir>"` (the `TreFiles` list already matches).
3. (For full NGE ships) add the complete SWGEmu client TREs to the dir AND to `TreFiles` (after the 26).
4. Verify integrity with `sha256sum -c tre_manifest.sha256`.
