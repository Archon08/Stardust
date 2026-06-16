# Stardust TRE Set — Missing Assets & Acquisition (for the full live project)

31 client assets are **referenced** by Stardust data but are **not present** in the 26-TRE repack (nor in
the pre-JTL retail disks). They were surfaced by the headless boot as
`TreeArchive: Did not find fileName: <x>`. **None affect the JTL space port** (verified: all JTL ships the
port spawns have their chassis appearances; all 10 space zones deploy with 0 spawn errors). They matter
only for a future **NGE-scope / full-ship-roster** project.

## Category A — NGE / special ship chassis appearances (24)
All 24 are **listed in `datatables/space/ship_chassis.iff`** but have no appearance `.iff` in the repack →
the ship type cannot spawn (cosmetic boot warning only). These are NGE-era or special ships:

```
ship_chassis_arc170                 ship_chassis_player_arc170
ship_chassis_tiedefender            ship_chassis_player_jedi_starfighter
ship_chassis_tiedefender_tier1..5   ship_chassis_player_test_falcon
ship_chassis_blacksun_medium_s01..04  ship_chassis_blacksun_transport
ship_chassis_merchant_cruiser_light ship_chassis_merchant_cruiser_medium
ship_chassis_hutt_turret_ship       ship_chassis_smuggler_warlord_ship
ship_chassis_mining_asteroid_dynamic ship_chassis_mining_asteroid_static
ship_chassis_rebel_shuttle          ship_chassis_transport_science
ship_chassis_generic
```
(Note: `blacksun_heavy`/`blacksun_light` + `basic_tiefighter` ARE present — only these specific variants are absent.)

## Category B — POB (boardable, multi-crew) ship assets (2)
```
corellian_corvette_pob.pob          shipcontrol_pob.iff
```
The player-boardable POB Corellian Corvette interior. **The Kessel encounter is unaffected** — it spawns
the AI corvette *agent* (chassis present), not the POB. POB corvettes are a separate player-owned feature.

## Category C — client ship definitions (.cdf) (3)
```
client_shared_blacksun_transport.cdf   client_shared_escape_pod.cdf   client_shared_hutt_transport.cdf
```

## Category D — misc (2)
```
particle_test_31.prt                shared_charric_carbine_schematic.iff
```

## Where these come from (acquisition for the full project)
All of the above ship the **complete retail / NGE SWG client** (≈ final Publish 14.1) — the standard
client SWGEmu players install via the official SWGEmu launcher (which patches a legally-owned retail SWG
install). This machine has only the **JTL-era MTG repack subset** + pre-JTL disks, which omit them.

**To close the gap for a live NGE-scope deployment (legitimately):**
1. Obtain the complete SWGEmu client TRE set (the official SWGEmu client your players use).
2. Place those base client TREs in the TrePath **below** (lower priority than) the 26 Stardust+MTG overlays.
3. Append them to `MMOCoreORB/bin/conf/config.lua` `TreFiles` **after** the 26 (so overlays still win).
4. Re-run the headless boot; the 31 `Did not find fileName` warnings should clear.

I did **not** download these (copyrighted SOE/LucasArts client assets; the legitimate path is the SWGEmu
launcher against owned retail). They are **not required** for the JTL space project, which is complete.
