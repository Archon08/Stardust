# Squadron Content Gaps — honest register for Live-faithful completion

**Scope.** This is the F3 content-gap register for the 9 pilot squadron chains. It records, per
squadron, the **current** in-repo content versus the **Live** (Pre-CU/JTL-era) objective from
`LIVE-REFERENCE-SCRAPBOOK.md`, and exactly what **original SWG data we do not have** would be needed to
make each chain fully Live-faithful.

**Hard rule (unchanged).** Use original SWG data; invent no balance and no mission content. Where a gap
cannot be closed faithfully from the corpus in hand, it is recorded here rather than fabricated.

**Live structure (scrapbook §2.2, §2.3, §2.4).** 3 factions × 3 squadrons = 9. Each squadron is a
self-contained themepark: a recruiter + a tiered mission chain (Tier 1→4, then a **Master** box),
culminating in the **Corvette Master Mission** — destroy a Corellian Corvette (capital ship) in
**Kessel** (Deep Space). Tier completion grants pilot **certification progression** (the
`setPilotTier` / `incrementPilotTier` path); the master tier is missions-only.

---

## 1. Current implementation state (audited)

Across `MMOCoreORB/bin/scripts/screenplays/space/squadrons/` and the squadron trainer conversation
handlers under `…/space/conversations/{rebel,imperial,neutral}/…`, only **3 of 9** squadrons have any
mission/trainer content at all:

| Faction | Squadron | Screenplay | Trainer convos | Tier chain present | Tier-cert wired |
|---|---|---|---|---|---|
| Rebel | **Akron's Havoc** | full (2.2k lines) | kreezo/viopa/aqzow/vrak | Tier 1–4 + Master | **Yes** (convo `incrementPilotTier` + screenplay `setPilotTier` — wired in F3) |
| Rebel | Vortex | **stub only** | none | none | n/a (no content) |
| Rebel | Crimson Phoenix | **stub only** | none | none | n/a (no content) |
| Imperial | Inquisition | **stub only** | none | none | n/a (no content) |
| Imperial | Storm | **stub only** | none | none | n/a (no content) |
| Imperial | Black Epsilon | **stub only** | none | none | n/a (no content) |
| Freelance | Smuggler Alliance | **stub only** | none | none | n/a (no content) |
| Freelance | RSF | full (1.3k lines) | dinge/kaydine/dulios | Tier 1–3 (no T4/Master) | Yes (Tier 1–3) |
| Freelance | CorSec | full (1.9k lines) | rhea/rikkh/ramna/turoldine | Tier 1–4 | Yes (Tier 1–4) |

"Stub only" = the screenplay file is the ~20-line empty `ScreenPlay:new{} … :start() end` skeleton with
**no missions, no recruiter conversation, and no quest data anywhere in the repo.** Wiring a tier grant
into these would require first **authoring the entire mission chain**, which the no-invent rule forbids.

---

## 2. Per-squadron gap detail

### 2.1 Tier-cert progression wiring (F3 — what was done vs deferred)

- **Havoc (rebel):** tier progression is granted on completion by the trainer conversation handlers
  (`kreezo/viopa/aqzow` call `ghost:incrementPilotTier()` gated on
  `SpaceHelpers:hasCompletedPilotTier(pPlayer, "rebel_navy", N)`). The screenplay's `reset*Quests`
  functions were **missing** the `setPilotTier(N)` reset call that the CorSec/RSF templates have, leaving
  pilot-tier state inconsistent after a quest reset. **Fixed in F3:** `setPilotTier(1..4)` added to
  `resetKreezoQuests/resetViopaQuests/resetAqzowQuests/resetArkonQuests`, matching the
  CorSec/RSF template exactly (faction key `rebel_navy`, tiers 1–4).
- **CorSec / RSF:** already wired; left unchanged.
- **The 6 stub squadrons:** cannot be wired — there is no tier chain, no trainer, and no completion hook
  point to attach a grant to. **Deferred:** requires authoring the chains (see §2.3).

### 2.2 Master mission = Corvette-in-Kessel (Live §2.4) — gap in ALL implemented chains

Per scrapbook §2.4 / checklist #7, **every** squadron's final mastery mission is to destroy a Corellian
Corvette in **Kessel** (Deep Space), with these Live rules:

- Corvette spawns on a ~2-hour timer on alternating hours; will not spawn while its **two gunboat
  escorts** sit at its exit point. Escorts only need to be **disabled** (engines killed) — they
  self-destruct after a minute or two.
- Named spawns (Scylla coords): Rebel **Corellian Corvette** exits at `-7260 4873 6341`; Imperial
  **Star Ravager** exits at `-6231 -259 -6059`.
- Completion requires holding the mission, being **in Kessel and in range when it dies**, and having
  **damaged a subsystem** (targeted with `[` / `]`).

Current in-repo master missions do **not** match this:

| Squadron | Current master mission | Live objective | Gap |
|---|---|---|---|
| Havoc (rebel) | `destroy_corellia_rebel_master`: generic "destroy **10** tier4/5 TIE" in `space_corellia`, 10 000 cr (`HavocSquadronScreenplay.lua` ~L1803) | Destroy a **Corellian Corvette in Kessel** w/ subsystem-damage + in-range completion | Wrong objective, wrong zone (`space_corellia` not Kessel/Deep Space), no Corvette spawn/escort logic, no subsystem-damage gate |
| CorSec (neutral) | Tier-4 chain ends in generic destroy/patrol/assassinate missions in `space_corellia`; no Corvette-in-Kessel master box | Per §2.2, CorSec is true-neutral: final mission vs Corvette, **player chooses** Rebel or Imperial Corvette | Missing the choice-of-target Corvette master entirely |
| RSF (neutral) | Chain stops at **Tier 3**; no Tier 4 and no master box | Per §2.2, RSF is heavily Imperial-aligned: final mission vs a **Rebel Corvette** | Missing Tier 4 + the Rebel-Corvette master |

### 2.3 The 6 stub squadrons — full chains missing

Vortex, Crimson Phoenix (rebel); Inquisition, Storm, Black Epsilon (imperial); Smuggler Alliance
(freelance) have **no content**. Each needs a complete Live-faithful themepark: recruiter +
4-tier mission chain + Corvette master. Faction alignment / recruiters / final-target from scrapbook §2.2:

| Squadron | Faction key | Recruiter & trainers (scrapbook §2.2) | Final Corvette target |
|---|---|---|---|
| Vortex | `rebel_navy` | Recruiter J'pai Brek (Tyrena); droid V3-FX, Moenia Naboo | Imperial Corvette (Star Ravager), Kessel |
| Crimson Phoenix | `rebel_navy` | Cmdr Da'la Socuna, Mos Espa Tatooine | Imperial Corvette (Star Ravager), Kessel |
| Inquisition | `imperial_navy` | Recruiter Cmdr Landau (Theed); Lt. Barn Sinkko, Kaadara Naboo | Rebel Corvette, Kessel |
| Storm | `imperial_navy` | Lt. Akal Colzet, Bestine Tatooine ("hardest squad in the game") | Rebel Corvette, Kessel |
| Black Epsilon | `imperial_navy` | Hakasha Sireen, Imperial Outpost Talus | Rebel Corvette, Kessel |
| Smuggler Alliance | `neutral` | Recruiter Gil Burtin (Bestine); Dravis, Mos Eisley (lightly Rebel-aligned) | Imperial Corvette, Kessel |

---

## 3. Original SWG data needed to close these gaps (do not invent)

To author the missing chains and the Corvette-in-Kessel master faithfully, the following **original
mission data** is required and is **not present** in the current corpus
(`SWG-DATA-CORPUS.md` covers ship/component datatables, not mission/quest definitions):

1. **Per-squadron mission definitions** for the 6 stub squadrons + the missing CorSec/RSF master boxes:
   the ordered mission list per tier (type: destroy/escort/patrol/inspect/assassinate/recovery/survival),
   target ship types/counts per mission, spawn locations/zones, and the per-mission reward (XP value,
   credits, any component/cert reward) — at **Live values**. SOE never published these; they live in the
   original `datatables/space/mission/*.iff` + the conversation `*.stf` strings.
2. **Recruiter & trainer conversation trees** (the `*.stf` dialog + the screen-branch logic) for each
   missing squadron, so the trainer can gate tier advancement and call `incrementPilotTier`.
3. **The Corvette / Star Ravager capital-ship encounter data:** the Corvette ShipAiAgent template
   (HP/subsystems/loadout), the **two gunboat escort** templates + their self-destruct-on-disable
   behavior, the Kessel/Deep-Space spawn schedule (~2 h, alternating hours, escort-gated), the spawn
   coordinates above, and the completion gate (in-Kessel + in-range-at-death + subsystem-damaged). This
   is a distinct **space encounter/objective system**, not just a destroy-N quest.
4. **Tier-XP requirements** per tier (T1/T2/T4 = XP + missions; T3 + Master = missions only): these live
   in the binary `skills.iff` (client TRE), confirmable only at boot (already flagged F1 in
   `FIDELITY-AUDIT.md`).

### Recommended sources to recover the above (for a later pass)
- Original SOE `space/` mission + datatable TREs (the authentic extraction track in `SWG-DATA-CORPUS.md`
  if mission tables are added to it).
- Live Stardust SQL snapshot (Drive) for any persisted squadron/mission state.
- SWGEmu / upstream Core3 squadron screenplays at the pin (`6856f31`) as a structural reference for the
  Corvette-in-Kessel encounter wiring — values still to be validated against the Live scrapbook, not
  copied as balance.

---

## 4. Status summary

- **Wired in F3 (faithful, from proven template):** Havoc tier-cert progression (`setPilotTier 1–4`).
- **Already wired (unchanged):** CorSec (T1–4), RSF (T1–3).
- **Deferred (cannot do faithfully without data we lack — recorded, not faked):**
  - Corvette-in-Kessel **master mission** for all chains (Havoc/CorSec/RSF + the 6 stubs).
  - **Full chains** for the 6 stub squadrons (Vortex, Crimson Phoenix, Inquisition, Storm,
    Black Epsilon, Smuggler Alliance).
  - RSF **Tier 4** + master.
