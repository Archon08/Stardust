# Kessel Corvette — Squadron Master Mission Reconstruction

> **Scope.** LUA content only (squadron Master tiers + a Kessel Corvette encounter screenplay) +
> scrapbook/TRE research + this doc. No C++/IDL ship-system code touched (that is a separate workstream).

## 1. Scrapbook facts (cited)

Corpus: `references/swg/Starborne/PreCUScrapbookv5.1/Biophilia's SWG PreCU Scrapbook v5.1`

Primary sources (Pilot forum captures), HTML stripped with `python`:

- **`data/20070127185616/index.html`** — "Corvette Master Mission Spawn Information" (MonsofoLexius, Scylla).
- **`data/20070127191311/index.html`** — "[GUIDE]/[FAQ] to Completing the Corvette Master Level Mission v3.0b" (Zina).

Documented Live behaviour:

| Fact | Documented value | Source |
|---|---|---|
| Location | Kessel (Deep-Space system) | both |
| Two corvettes, one per faction | Rebel = **Corellian Corvette**; Imperial = **Star Ravager** | both |
| Imperials destroy | the **Rebel** Corellian Corvette | both |
| Rebels destroy | the **Imperial** Star Ravager | both |
| Freelancers/Smugglers | no master trainer of their own — take the Rebel **or** Imperial master quest at that trainer, destroy the matching corvette (no Freelancer corvette) | Zina FAQ |
| Spawn timer | ~2 hour timer; the two vettes alternate ~1 hour apart | both |
| In-system dwell | corvette hyperspaces out ~**45 min** after spawn; ~1h15m to respawn | Zina FAQ |
| Escorts | **2 tier-5 gunboats** + a few fighter escorts; gunboats orbit a single point at the exit | both |
| Escort self-destruct | disabled gunboats (engines out) **self-destruct after a minute or two** | MonsofoLexius |
| Completion gate | must be **in Kessel when the corvette is destroyed** **and** have done **damage to ≥1 subsystem** this trip (disputed in-thread; SOE guidance: "just damage the vette") | both |
| Subsystems | 2 shields + 8 turbo lasers (numbered 0–7); weapons can be killed without dropping shields first | Zina FAQ |
| Death handling | you do **not** lose the mission on death; clone-zerg allowed | both |
| Kessel access | Rebel → Dantooine deep-space station → /comm "kessel"; Imperial → Endor deep-space station; Freelancer → Dathomir neutral station | Zina FAQ |
| Documented exit coords (Scylla) | Rebel Corvette `-7260 4873 6341`; Star Ravager `-6231 -259 -6059` | MonsofoLexius |
| Documented spawn coords (Naritus) | Star Ravager `7340 7550 6268`; Corellian Corvette `4870 -5056 -4765` | Zina FAQ |
| Tier | everything in Kessel is **Tier 5** | Zina FAQ |

**Unknown / not documented:** exact reward (the Zina "VI. Rewards" section is on an un-archived
continuation page); exact subsystem-damage threshold (in-thread dispute, never confirmed); exact escort
fighter count ("a few"); precise self-destruct timing ("a minute or 2").

## 2. Authentic TRE data (decisive)

From `mtg_patch_013_configurable_02.tre` (read via `tre_read.py`), the Master mission is a **two-stage
chain** per faction — `master_*_1` then `master_*_2`:

- **`master_imperial_1` / `master_rebel_1`** (stage 1) — *"Travel to the Kessel System"* + *"Destroy
  Thirty {Rebel|Imperial} Fighters"*. Target lists (verbatim from the `.stf`):
  - imperial_1: X-Wings, B-Wings, A-Wings, Y-Wings, **Rebel gunboats** (non-capital craft).
  - rebel_1: TIE Advanced, TIE Aggressor, TIE Interceptor, TIE Oppressor, **Imperial Gunboats**.
- **`master_imperial_2` / `master_rebel_2`** (stage 2) — *"Destroy the {Rebel|Imperial} Corellian
  Corvette"*: *"In order to destroy a capital ship you must target and destroy each sub-component of the
  ship."* This is the Corvette Master encounter.

Files: `datatables/quest{task,list}/spacequest/destroy/master_{imperial,rebel}_{1,2}.iff` +
`string/en/spacequest/destroy/master_{imperial,rebel}_{1,2}.stf`. Each task has the same 2-task internal
structure (location task → destroy task), which maps 1:1 onto `SpaceDestroyScreenplay`.

In-repo assets already present (all confirmed):
`ship_mobile/ships/imp_corellian_corvette_tier4.lua` (capital, imperial faction = the Star Ravager
target), `reb_corellian_corvette_tier4.lua` (capital, rebel faction = the Corellian Corvette target),
`imp_imperial_gunboat_tier5.lua` / `reb_gunboat_tier5.lua` (tier-5 escorts), plus the full Kessel loot
families and `ship_chassis_corvette.iff`.

## 3. What was authored

**New file** `screenplays/space/squadrons/KesselMasterEncounterScreenplay.lua`:

- `KesselCorvetteEncounter` — base extending `SpaceDestroyScreenplay`. On zone entry it spawns the faction
  corvette + 2 tier-5 gunboat escorts (via `spawnShipAgent`, the same API used by
  `SpaceSurpriseAttackScreenplay`) at the documented Kessel exit geometry, then reuses the proven destroy
  gate: **completion fires only when the player is in the quest zone AND destroys the corvette**.
- **Stage 1** `destroy_master_imperial_1` / `destroy_master_rebel_1` — authentic 30-fighter
  seek-and-destroy with the **faction-correct target lists from the TRE strings** (replaces the previous
  bare placeholder: kill-10 generic TIEs).
- **Stage 2** `destroy_master_imperial_2` / `destroy_master_rebel_2` — the Corvette encounter.
  Imperial pilots hunt `reb_corellian_corvette_tier4`; Rebel/Smuggler pilots hunt
  `imp_corellian_corvette_tier4`.

**Edited (9 squadron Master tiers → 7 with a master tier; CorSec & RSF have none):**

- Imperial: `BlackEpsilon`, `Inquisition`, `Storm` → Master tier now points at `master_imperial_1` (stage
  1) + new `master_imperial_2` (corvette), with both staged into reset.
- Rebel: `CrimsonPhoenix`, `Vortex` and the `Smuggler` (freelancer-rebel) squadron → `master_rebel_1` +
  new `master_rebel_2`.
- `Havoc` — repointed from its bespoke non-TRE `corellia_rebel_master` placeholder to the **authentic**
  `master_rebel_1` + `master_rebel_2` chain (Havoc is Rebel → hunts the Imperial corvette).

The duplicate per-squadron placeholder `destroy_master_*` object definitions were removed; the master
objects now live once in the encounter file (loaded first via `screenplays/space/screenplays.lua`), so the
squadrons' existing `:resetQuest` references resolve unchanged. `KesselMasterEncounterScreenplay\.lua`
added to `tools/fidelity/authored-allowlist.txt`.

## 4. Fidelity rating: **MEDIUM-HIGH**

Faithful: two-stage structure, faction targeting, target ship lists, the capital-corvette + 2 tier-5
gunboat escort composition, the "be in-system + destroy the corvette" completion gate, and the documented
exit coordinates — all sourced from authentic TRE data and primary scrapbook captures, no invented
balance.

## 5. Residual gap (honest)

1. **No `space_kessel` zone.** This server build has **no Kessel terrain/region/patrol** zone (terrain
   TREs ship corellia/dantooine/dathomir/endor/naboo/tatooine/yavin4/lok/kashyyyk/heavy1/light1 only).
   Kessel was a dynamic Deep-Space instance in Live. To avoid fabricating an unsupported zone, the
   encounter currently spawns in each quest's existing registered `questZone` using the documented Kessel
   geometry; the canonical zone is parameterised as `KESSEL_TARGET_ZONE` (empty ⇒ use questZone). **Closing
   data:** the Kessel space terrain `.trn` + region/patrol-point definitions. Once added, set
   `KESSEL_TARGET_ZONE = "space_kessel"` and the encounter is Kessel-true.
2. **Subsystem-damage gate is approximated.** `SpaceDestroyScreenplay`'s `DESTROYEDSHIP` observer fires on
   whole-ship destruction; it cannot require "damaged ≥1 subsystem this trip." The in-range + destroy gate
   is reproduced faithfully; the subsystem-damage precondition is **not** enforced (and was itself disputed
   in Live). **Closing data:** a subsystem-hit observer hook on the corvette agent (engine-side).
3. **Spawn timer / 45-min dwell / escort self-destruct not reproduced.** The corvette spawns on zone entry
   for the mission holder rather than on a global ~2h alternating timer with a 45-min hyperspace-out and
   disabled-gunboat self-destruct. **Closing data:** the original Kessel global-spawn timer values and the
   escort self-destruct trigger (not recoverable from the corpus — only "~2 hours" / "~45 min" / "a minute
   or 2" approximations are documented).
4. **Reward unknown.** Stage-2 credit reward is a placeholder (25000); the authentic master reward is not
   in the archived scrapbook page and was not located in the TRE strings (only auto-reward STF keys exist,
   empty here). **Closing data:** the master mission reward row / autoreward STF body.
