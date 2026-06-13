# Squadron Reconstruction Notes — Inquisition (Imperial), proof-of-concept

**Task.** Reconstruct ONE space squadron themepark chain (the **Inquisition** Imperial squadron) as a
Live-faithful proof-of-concept, mirroring the three complete in-repo squadrons (Havoc/CorSec/RSF), and
keep `space-lint.yml` green / `space-port-build.yml` unbroken.

**Outcome (headline).** The Inquisition chain is **fully specified and structurally recoverable**, but it
**cannot be committed into `MMOCoreORB/bin/scripts/screenplays/space/` on this branch without failing
`space-lint.yml`.** That workflow contains a hard **Content-fidelity gate** that holds the entire
`screenplays/space/` (and `mobile/conversations/space/`, etc.) tree **byte-for-byte identical to upstream
Core3 pin `6856f315a80b5250635b2272695caec1d64204ed`**, where Inquisition is an empty stub. Authoring the
screenplay/convo-handler is therefore a guaranteed lint failure — and was already attempted-and-reverted
on this branch (see "Precedent" below). This document records the complete reconstruction spec, the
evidence that the chain is recoverable, the exact blocker, and the unblock path, so the work can land the
moment the fidelity gate is updated.

---

## 1. The blocker, precisely

`.github/workflows/space-lint.yml` step **"Content fidelity — every dropped space dir must equal upstream
pin"** runs, for each space dir including `screenplays/space`:

```
diff -rq /tmp/core3/MMOCoreORB/bin/scripts/screenplays/space  MMOCoreORB/bin/scripts/screenplays/space
```

against the tarball of Core3 @ `CORE3_PIN=6856f315a80b5250635b2272695caec1d64204ed`. Any difference fails
the job.

- `InquisitionSquadronScreenplay.lua` in this repo is **byte-for-byte identical** to the upstream pin
  (verified: `diff` is empty; it is the 21-line `ScreenPlay:new{} … :start() end` stub).
- Editing the stub → `diff` reports "files differ" → **FAIL**.
- Adding `screenplays/space/conversations/imperial/inquisition_squadron/*.lua` → `diff -rq` reports
  "Only in …" → **FAIL**.
- The recruiter NPC convo binding would also go under `mobile/conversations/space/imperial/…`, which is
  **also** in the fidelity-locked dir list → same failure.

There is no placement of squadron content inside the locked dirs that survives the gate.

### Precedent on this very branch
The git log shows this was already learned the hard way:
- `46f7f63a` "wire rebel_navy pilot tier grant (setPilotTier 1-4) into Havoc squadron reset funcs" —
  added content to a squadron screenplay.
- `3b68b0f9` "**revert** Havoc setPilotTier change — restore exact upstream Core3@6856f31 bytes …
  adding it diverges from Live."
- `2e84a224` corrected `SQUADRON-CONTENT-GAPS.md` to state squadron screenplays are fidelity-locked to the
  pin and "all gaps are missing original content."

`space-port-build.yml` is not the constraint (it is a C++ CMake/Ninja build, `paths-ignore: docs/**`, and
triggers on `space-port/**` pushes); a Lua/doc change does not break it. The binding gate is the lint.

---

## 2. Inquisition spec — what came from the FAQ (cited)

Primary source: **Pilot FAQ v3.0**, Biophilia's SWG Pre-CU Scrapbook v5.1,
`…/data/20070204121749/index.html` ([OFFICIAL-EP], authored by the official pilot correspondent).
HTML-stripped quote, verbatim:

> "Imperial: Recruiter Location: Commander Landau (-5516,4403) Theed, Naboo
> **Inquisition: Lt. Barn Sinkko, Kadaara, Naboo (5204, 6728)**
> Storm Squadron: Lt. Akal Colzet, Bestine, Tatooine (-1110, -3514) …
> Black Epsilon: Hakasha Sireen, Imp[erial Outpost, Talus]"

And, for the Imperial final mission (same FAQ, RSF/alignment passage; corroborated scrapbook §2.4):

> Imperial squads' final mission is against a **Rebel Corvette** in **Kessel** (Deep Space).

Confirmed Inquisition facts (FAQ + scrapbook §2.2, identical):
- **Faction:** Imperial (`imperial_navy` cert tree).
- **Recruiter:** Commander Landau, Theed, Naboo `(-5516, 4403)` — shared Imperial recruiter.
- **Squadron trainer:** Lt. Barn Sinkko, Kadaara (Kaadara), Naboo `(5204, 6728)`.
- **Tier gating (FAQ, verbatim):** "Tier 1: xp and missions / Tier 2: xp and missions / Tier 3: missions
  only / Tier 4: xp and missions / Master Box: missions … only." Post-master → faction prestige →
  Deep Space.
- **Master objective:** destroy a Rebel Corellian Corvette in Kessel (subsystem-damaged + in-range-at-death).

What the FAQ does **NOT** contain (verified by grep — `mission 1`, `destroy mission`, `escort mission`,
`patrol mission` all absent): the per-mission chain — i.e. the ordered mission list per tier, target ship
types/counts, spawn coordinates, and per-mission rewards. The FAQ gives recruiter/trainer metadata, tier
rules, and the master objective only. **No per-mission balance values exist in the scrapbook corpus.**

---

## 3. What is templated from the working squadrons (structure)

The proven SOE-derived pattern (Havoc/CorSec/RSF) is a 3-part structure. Inquisition would mirror it
exactly:

1. **`InquisitionSquadronScreenplay.lua`** — defines each tier's missions as
   `Space{Patrol,Destroy,Escort,Assassinate,Recovery,Inspect,Survival,…}Screenplay:new{…}` objects
   (questName/questType/questZone/creditReward/itemReward/spawn data), each `registerScreenPlay(name,true)`;
   then `InquisitionSquadronScreenplay = ScreenPlay:new{ QUEST_STRING_* = {type=,name=} … }` plus
   `reset<Trainer>Quests(pPlayer)` helpers. (Cf. `HavocSquadronScreenplay.lua` L15–1988 missions,
   L1995–2056 quest-strings, L2065+ reset funcs.)
2. **Trainer convo handler(s)** under `screenplays/space/conversations/imperial/inquisition_squadron/`
   (e.g. `barnSinkkoConvoHandler.lua`) — a `conv_handler:new{}` with `getInitialScreen` (faction + pilot-tier
   gating: `SpaceHelpers:isImperialPilot`, `…:isInquisitionSquadron`, quest-active/complete branching to
   conversation screens) and `runScreenHandlers` (starts quests, grants rewards, calls
   `SpaceHelpers:grantSpaceSkill(...)` and **`ghost:incrementPilotTier()`** on
   `SpaceHelpers:hasCompletedPilotTier(pPlayer,"imperial_navy",N)` — the Live tier-grant mechanism, NOT
   `setPilotTier`). This mirrors `kreezoConvoHandler.lua` 1:1, swapping `rebel`→`imperial`,
   `HavocSquadronScreenplay`→`InquisitionSquadronScreenplay`, `pilot_rebel_navy_*`→`pilot_imperial_navy_*`,
   `HAVOC_SQUADRON`→an `INQUISITION_SQUADRON` constant.
3. **Recruiter NPC binding** under `mobile/conversations/space/imperial/inquisition_squadron/` (cf.
   `mobile/conversations/space/rebel/havoc_squadron/kreezo_convo.lua`) tying the NPC to the handler.
4. **Registration** in `screenplays/space/screenplays.lua` (an `includeFile("…/InquisitionSquadronScreenplay.lua")`
   and the convo-handler include) and the `mobile`/`serverobjects` include chain — mirroring how Havoc's
   are wired. (All of these target files are inside fidelity-locked dirs.)

Mechanisms confirmed in template (so the reconstruction would reuse, not invent): tier cert granted via
**trainer convo `incrementPilotTier`** gated on `hasCompletedPilotTier(…,N)`; reward-once via
`getQuestStatus/setQuestStatus(playerID..name..":reward")`; faction standing via
`ghost:increaseFactionStanding("imperial", N)`; quest start via `<mission>:startQuest(pPlayer,pNpc)`.

---

## 4. What dialogue would be reconstructed

The recruiter/trainer dialogue is **not authored in Lua** — the handlers reference client STF strings
(`@conversation/<file>:s_<hash>`) that live in TRE-loaded conversation templates (e.g. Havoc references
`@conversation/corellia_rebel_trainer_1:…`). The Imperial equivalents (`@conversation/*_imperial_trainer_*`
/ the Inquisition trainer's STF) are **client-data**, not in this repo. So the convo handler is mostly
control-flow over data-driven strings; the only "reconstructed" prose would be any branch the Imperial STF
doesn't supply, kept minimal and consistent with the template (Imperial tone: orders/duty framing rather
than the Rebel "join the cause" framing). No new mechanics or reward economies would be introduced.

---

## 5. Evidence the chain IS recoverable at high fidelity (the real ceiling)

The Inquisition mission chain is **data-driven** and the data **exists** in the client TREs. Extracted
`datatables/(questlist|questtask)/spacequest` from `mtg_patch_013_configurable_02.tre`
(`/mnt/c/Stardust-TREs/…`) and confirmed a **complete Imperial mission set**, planet-keyed exactly like the
working squadrons (Havoc=`corellia_rebel_*`, RSF=`naboo_privateer_*`, CorSec=`corellia_privateer_*`):

- **`naboo_imperial_*`** — full chain: Tier 1 (`patrol/naboo_imperial_1`, `destroy/naboo_imperial_2`,
  `escort/naboo_imperial_3`, `assassinate/naboo_imperial_4`, duty `_6/_7`), Tier 2 (`*_tier2_*`), Tier 3
  (`*_tier3_*`), Tier 4 (`*_tier4_*`), plus `destroy/master_imperial_1`, `master_imperial_2`.
- Sibling Imperial families `corellia_imperial_*`, `tatooine_imperial_*` (the other two Imperial squads),
  and station/story variants (`imperial_ss_*`, `naboo_station_emperors_access_*`).

Because the Inquisition recruiter (Theed) and trainer (Kaadara) are **on Naboo**, the Inquisition chain
maps to the **`naboo_imperial_*`** family — directly analogous to Havoc←`corellia_rebel`.

The `questtask` tables are real **DTII datatables** with columns
`PREREQUISITE_TASKS / EXCLUSION_TASKS / ATTACH_SCRIPT / JOURNAL_ENTRY_TITLE / JOURNAL_ENTRY_DESCRIPTION /
TASKS_ON_COMPLETE / TASKS_ON_FAIL / TARGET / PARAMETER / IS_VISIBLE / TYPE` and `@spacequest/<type>/<name>:…`
journal STF refs — i.e. the authentic, server-loadable mission definitions the screenplays reference by
name. These are loaded from TRE at runtime; the screenplay only needs to reference the names + supply the
spawn/reward wiring, exactly as Havoc does.

**Fidelity ceiling for Inquisition: HIGH.** Recruiter/trainer/coords/faction/tier-rules/master-objective
are FAQ-confirmed [OFFICIAL-EP]; the full mission chain is recoverable from the `naboo_imperial_*` TRE
datatables at original values; the code structure is a 1:1 template port from Havoc. The only genuine
reconstruction is incidental convo-branch prose where the Imperial STF is silent (minimal, no invented
mechanics). The remaining hard gap shared by ALL squadrons is the **Corvette-in-Kessel master encounter**
(capital-ship spawn schedule, gunboat-escort self-destruct, subsystem-damage completion gate), which is a
distinct space-encounter system not present as a template anywhere in-repo (see `SQUADRON-CONTENT-GAPS.md`
§2.2).

---

## 6. Can the other 5 stubs reach the same standard? (ceiling before scaling)

| Squadron | Faction | Recruiter/trainer (FAQ) | TRE mission family present? | Ceiling |
|---|---|---|---|---|
| **Inquisition** | Imperial | Landau/Theed; Lt. Barn Sinkko, Kaadara Naboo | **Yes** `naboo_imperial_*` (T1–4 + master_imperial) | **HIGH** |
| **Storm** | Imperial | Lt. Akal Colzet, Bestine Tatooine | **Yes** `tatooine_imperial_*` (T1–4) | **HIGH** (FAQ: "hardest squad"; values still from data, not invented) |
| **Black Epsilon** | Imperial | Hakasha Sireen, Imp Outpost Talus | **Yes** `corellia_imperial_*` (T1–4) — third Imperial family | **HIGH** (NB: recruiter is on Talus but mission family is the remaining `corellia_imperial_*`; confirm mapping at author time) |
| **Vortex** | Rebel | droid V3-FX, Moenia Naboo | Likely (`vortex_mission_*` tables seen in extract; + a rebel family) | **MED–HIGH** (verify which rebel family is unused by Havoc) |
| **Crimson Phoenix** | Rebel | Cmdr Da'la Socuna, Mos Espa Tatooine | Likely `tatooine_rebel_*` (seen in extract) | **MED–HIGH** |
| **Smuggler Alliance** | Freelance | Dravis, Mos Eisley | Likely `tatooine_privateer_*` / `*_sa` (seen in extract) | **MED–HIGH** |

**Assessment.** All three **Imperial** squads (Inquisition, Storm, Black Epsilon) can be reconstructed to
the SAME standard as Havoc/CorSec/RSF: the three planet-keyed `*_imperial_*` mission families exist in the
TRE with full T1–4 + `master_imperial_*`, the recruiters/trainers/coords are FAQ-confirmed, and the code is
a template port. The two remaining **Rebel** stubs (Vortex, Crimson Phoenix) and **Smuggler Alliance**
also have candidate mission families in the same TRE extract (`vortex_mission_*`, `tatooine_rebel_*`,
`tatooine_privateer_*`/`*_sa`) — high likelihood, but the exact family↔squadron mapping must be confirmed
per squad before authoring (Havoc already consumes one rebel family; the others must be the unused ones).

**Real ceiling, honestly:** the per-tier *mission chains* for all 6 stubs are recoverable at HIGH fidelity
from the TRE + FAQ. The one component that is **MED-LOW everywhere** (Havoc/CorSec/RSF included) is the
**Corvette-in-Kessel Master mission** — no in-repo template, distinct encounter system, requires the
capital-ship/escort/Kessel-spawn data (`SQUADRON-CONTENT-GAPS.md` §3). So: 4 tiers per squad = HIGH and
scalable; the Master box = a separate, shared engineering task gated on recovering the Corvette encounter
data.

---

## 7. Unblock path (to actually land this content, lint-green)

The content is ready to author the instant the fidelity gate stops diff-locking authored squadrons. Pick
one, in order of cleanliness:

1. **Scope the fidelity `diff -rq` to exclude intentionally-authored squadron paths** (an allowlist /
   `--exclude` for `squadrons/`, `conversations/{imperial,…}/<squad>_squadron/`, and the matching
   `mobile/conversations/space/…/<squad>_squadron/`), so authored content is permitted while the rest of
   `screenplays/space` stays pinned. This keeps the lint meaningful (still pins everything else) and lets
   Inquisition land.
2. **Bump `CORE3_PIN`** to an upstream commit that actually ships these squadrons (if one exists), or to a
   Stardust fork pin that includes them.
3. Move authored squadron content to a non-locked tree (not recommended — breaks the server's expected
   `screenplays/space/` load layout).

Once (1) or (2) is in place, deliver: `InquisitionSquadronScreenplay.lua` (naboo_imperial_* chain),
`screenplays/space/conversations/imperial/inquisition_squadron/barnSinkkoConvoHandler.lua`,
`mobile/conversations/space/imperial/inquisition_squadron/barn_sinkko_convo.lua`, and the `screenplays.lua`
+ `serverobjects`/mobile include edits — all mirrored from the Havoc set.

---

## 8. Provenance summary

- **From FAQ [OFFICIAL-EP] `20070204121749/index.html`:** recruiter (Landau/Theed -5516,4403), trainer
  (Lt. Barn Sinkko, Kaadara 5204,6728), Imperial faction, tier-training rules, Rebel-Corvette-in-Kessel
  master objective.
- **From in-repo templates (Core3 pin):** all code structure — screenplay mission-object pattern,
  quest-string table, reset helpers, convo `getInitialScreen`/`runScreenHandlers` gating, tier grant via
  `incrementPilotTier` + `hasCompletedPilotTier(…,"imperial_navy",N)`, reward-once status keys, faction
  standing. (Mirrors `HavocSquadronScreenplay.lua` + `kreezoConvoHandler.lua`.)
- **From TRE `mtg_patch_013_configurable_02.tre`:** the actual mission chain data
  (`datatables/(questlist|questtask)/spacequest/.../naboo_imperial_*` + `master_imperial_*`), loaded at
  runtime — DTII datatables with TARGET/PARAMETER/reward/journal columns.
- **Reconstructed (minimal):** only incidental convo-branch prose where the Imperial conversation STF does
  not supply a line; consistent with template, no new mechanics/economy.
- **Honest gap (shared by all squads):** Corvette-in-Kessel Master encounter data/system — deferred, not
  faked (see `SQUADRON-CONTENT-GAPS.md`).
