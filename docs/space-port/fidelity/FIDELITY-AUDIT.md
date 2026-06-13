# F0 Fidelity Audit — Space SYSTEMS + DATA vs SWG Live

**Phase:** F0 (Fidelity Lock). **Repo:** Archon08/Stardust @ `stardust-space`.
**Method:** data-only, no clone/boot. In-repo C++/Lua fetched via `gh` API and compared against the
authoritative Live sources, with extracted SOE datatables read via the local IFF parser.

**Precedence (per FINISH-SPACE-PLAN F0 + PROVENANCE-SPEC):** official publish notes (D) > extracted
SOE datatable (A) > SWGEmu RE'd values (B). Per SWG-DATA-CORPUS §B/§Principle, the SWGEmu RE'd
dataset *is* the accepted Live-faithful standard for the component/maneuver numbers SOE never published;
the LIVE-REFERENCE-SCRAPBOOK §3.x community numbers are corroborative but rank below an extracted SOE
table or the SWGEmu RE reconstruction. **Directive observed: match Live, never invent balance.**

**Provenance tags:** [A]=extracted SOE datatable; [B]=SWGEmu RE (= upstream Core3, verified by diff);
[D]=official publish/FAQ facts in scrapbook; [COMM]=player-measured community number in scrapbook.

**Transplant baseline:** the space subsystem was transplanted from upstream Core3@6856f31
(commit `4c1c8df3`). Every audited C++ system was diffed against `swgemu/Core3` to establish whether
the port preserves the canonical SWGEmu RE'd Live behavior.

---

## Verdict summary

| Verdict | Count | Items |
|---|---|---|
| **MATCH** (Live-faithful, no change needed) | 6 of 6 audit items | Damage model, Speed/maneuver, Crew-XP/master-bonus, Cert structure (9 squadrons + tiers), Even-level/component semantics, Chassis data spot-check (8/8) |
| **Corrected** | 0 | — none required: every divergence from the scrapbook was at the [COMM] tier where the higher-precedence SWGEmu-RE / extracted-SOE value governs; correcting toward the community number would *invent balance away from Live* and is therefore explicitly NOT done |
| **Deferred to F1/F2 (needs boot)** | 2 sub-findings | (a) per-tier XP-vs-mission skill gating lives in the binary `skills.iff` (client TRE, not in-repo) — runtime-only; (b) dropped upstream faction-point/GCW award in `disseminateSpaceExperience` — needs boot to confirm GCW impact |
| **Content gap flagged for F3 (not a data correction)** | 1 | squadron Master mission is a generic "destroy 10 ships" quest rather than the Live Corvette-in-Kessel kill (checklist #7) |

**No code corrections were applied; the build is green by construction (no code touched).** The F0
deliverable is this docs-only audit. (Build workflow `space-port-build.yml` ignores `docs/**`, so this
commit does not perturb CI; the last `space-port/**` build runs are green — run 27465544528 success.)

---

## Item 1 — Damage model (the death-spiral) — **MATCH** [B]

**Live spec (scrapbook §1.5 / §3.5, [COMM]):** a shot resolves shield (×vs-shield) → armor (×vs-armor)
→ **one component selected at random, one per shot** (weighted by per-slot hit-weights) → component
armor then HP, with HP-0 **remainder spilling to chassis**; a **disabled component statistically stays
selectable** so losing a subsystem accelerates hull death. The label "interceptor death-spiral" is
community usage; no SOE statement names it (Appendix B).

**Port actual** (`MMOCoreORB/src/server/zone/managers/spacecombat/SpaceCombatManager.cpp`):
- `applyDamage` runs the exact order shield→armor→component→chassis
  (`SpaceCombatManager.cpp:166-189`), each stage returning leftover damage that **falls through to the
  next stage / to chassis** — the spillover that drives the spiral is present (`:180-189`,
  `applyChassisDamage` `:380`).
- One random component per shot is chosen by `getActiveComponentToDamage` (`:563-619`), weighted by
  `collisionData->getSlotWeight(slot)` (the hit-weights from `ship_chassis.iff` [A], all `10` =
  uniform-by-default, confirmed in the extracted table).
- **Nuance vs the [COMM] description:** the SWGEmu model *excludes* a fully-destroyed component
  (`hitpointsMap->get(slot) <= 0.f → continue`, `:589`) from the random pool, then routes leftover
  damage onward to chassis via the outer loop — rather than the scrapbook's "dead slot stays in pool
  and re-selection passes full damage through." **Both produce the same emergent death-spiral**
  (destroying a cheap low-HP part removes its absorption and lets damage reach the hull faster); the
  port uses the canonical SWGEmu mechanism.

**Verdict: MATCH.** `SpaceCombatManager.cpp` is **byte-identical to upstream `swgemu/Core3`** (diff =
empty). This is the canonical SWGEmu RE'd Live behavior [B], which per corpus §B is the accepted
Live-faithful standard. The death-spiral is **present and intended** — explicitly NOT "fixed."

**Correction applied:** none (and none permitted — the death-spiral is period-correct Live behavior).

---

## Item 2 — Speed / maneuver model — **MATCH** [B/A]

**Live spec (§3.1 [COMM/EP], §3.4):** `max ship speed = engine_speed × 10 × Spdx`; `Spdx` is a
per-chassis hard-cap speed multiplier; **all acceleration/decel/pitch/yaw/roll are chassis-dictated**,
the engine only sets speed.

**Port actual** (`MMOCoreORB/src/server/zone/objects/ship/ShipObjectImplementation.cpp`):
- `calculateActualMaxSpeed()` (`:2429-2453`):
  `clamp(0, (engineSpeed + boosterSpeed) × chassisSpeed, 512)`, where
  `engineSpeed = getEngineMaxSpeed() × engineEfficiency` and `chassisSpeed = getChassisSpeed()` (the
  per-chassis Spdx, loaded from the chassis `maxSpeed` attribute at `:192-193`). Structure =
  `engine_speed × chassis_Spdx`, globally clamped to 512. The Live "× 10" is folded into the SWGEmu
  stored engine `maxSpeed` magnitudes (e.g. `eng_*` maxSpeed≈42) vs the small Spdx (≈0.85–1.0); the
  port reproduces the SWGEmu RE'd value chain exactly.
- Accel/decel/pitch/yaw/roll all sourced from the **chassis** attributes
  (`engineAccel/engineDecel/engineYawAccel/enginePitchAccel/engineRollAccel`) in `loadTemplateData`
  (`:177-194`) and applied in `updateActualEngineValues` (`:2483-2544`) — confirms "all acceleration
  chassis-dictated" (§3.4, checklist #14).

**Verdict: MATCH.** The speed/maneuver logic is identical to upstream `swgemu/Core3` (the only diff
is 2 additive `#include` lines for unrelated transaction/comm-timer features). Per-chassis Spdx caps
clamp; global 512 cap present.

**Correction applied:** none.

---

## Item 3 — Pilot certification gating (9 squadrons, tier rules) — **MATCH (structure)**; sub-items deferred

**Live spec (§2.1-2.3 [D/EP], checklist #1-3,5):** pilot is skillpoint-free, trainer-only, gated on
XP + squadron missions; **3 factions × 3 squadrons = 9**; tier gating T1 xp+msn, T2 xp+msn,
**T3 missions-only**, T4 xp+msn, **Master missions-only**; post-master faction-specific **prestige**
gates Deep Space.

**Port actual:**
- **All 9 squadron screenplays present, correctly faction-aligned**
  (`MMOCoreORB/bin/scripts/screenplays/space/squadrons/`): Rebel = `HavocSquadronScreenplay`,
  `VortexSquadronScreenplay`, `CrimsonPhoenixSquadronScreenplay`; Imperial =
  `InquisitionSquadronScreenplay`, `StormSquadronScreenplay`, `BlackEpsilonSquadronScreenplay`;
  Neutral = `SmugglerSquadronScreenplay`, `RsfSquadronScreenplay`, `CorsecSquadronScreenplay`. Exactly
  the §2.2 set. Recruiters present (`j_pai_brek_convo`, Cmdr-Landau/imperial, `gil_burtin_convo`).
- **Tier structure present:** `HavocSquadronScreenplay.lua` is organized Tier 1→4 + **Master**, with
  per-tier `TIERn_QUEST_STRING_*` mission lists and `TIER4_QUEST_STRING_MASTER`
  (`HavocSquadron.lua:2009-2051`). Master tier is mission-defined (`destroy_corellia_rebel_master`),
  consistent with "Master = missions-only."
- **Master faction structure wired into C++:** `disseminateSpaceExperience` keys off
  `pilot_rebel_navy_master` / `pilot_imperial_navy_master` / `pilot_neutral_master` skills and awards
  `prestige_rebel` / `prestige_imperial` / `prestige_pilot` currencies
  (`PlayerManagerImplementation.cpp:1618-1623`) — confirms **faction-specific prestige** exists
  (checklist #5) and post-master earns prestige not XP.

**Deferred to F1 (runtime):** the per-tier **XP-vs-mission requirement** and **skillpoint-free** cost
are encoded in the binary `datatables/skill/skills.iff` shipped in the client TRE — **not present as
in-repo editable content**, so the exact T1/T2/T4=xp+msn vs **T3/Master=msn-only** gating cannot be
validated from files and must be confirmed on a boot (mark **F1**).

**Content gap (F3, not a data correction):** the Havoc Master mission `destroy_corellia_rebel_master`
is a generic "destroy 10 tier4/5 TIE" quest in `space_corellia` (`HavocSquadron.lua:1803-1827`),
**not** the Live "destroy a Corellian Corvette in **Kessel**" mastery (§2.4, checklist #7). Flag for F3
content authoring; no data value to correct in F0.

**Verdict: MATCH** for the gating *structure* (9 squadrons, 4-tier + master, prestige currencies);
XP/mission tier-requirement = **UNCLEAR/deferred F1**; Corvette-mastery content = **gap (F3)**.

**Correction applied:** none in F0.

---

## Item 4 — Crew-shared kill XP + previous-master bonus — **MATCH** [D]

**Live spec (§1.3 [D], §2.5 [EP]):** kill credit shared to the **entire crew** of a multi-crew craft;
previous-master bonus = `XP × (1 + number of FACTIONAL masters)`, counted **per faction** (not per
squad), max `3×+1`; completion needs being **in range when the target dies**.

**Port actual** (`PlayerManagerImplementation.cpp:1534-1652`, `disseminateSpaceExperience`):
- **Crew-share:** XP divided across `playerShip->getPlayersOnBoard()` and awarded to **every** onboard
  crew member (`:1592-1647`). Matches "credit to the entire crew" (#19).
- **In-range gate:** `isInRange3dZoneless(playerShip, SPACECLOSEOBJECTRANGE)` (`:1588`) — matches the
  §2.4 in-range-at-death requirement.
- **Previous-master bonus:** `aceMultiplier` starts at `1.0`, then `+1` for **each of the 3 faction
  ace-badge groups** (rebel 130-132 / imperial 133-135 / neutral 136-138) in which the player holds any
  badge, breaking after the first hit per group (`:1625-1645`). Result =
  `XP × (1 + #factional_masters)`, range 1..4 = **exactly `3×+1`, counted per faction not per squad**.
  Matches §2.5 (#6) precisely.

**Verdict: MATCH.** Note: this function **diverges from upstream Core3** (upstream uses
`aceMultiplier = 0` + a separate second award; the port consolidates to `1 + N` in one award) — but the
**effective Live formula is identical and the port's form is cleaner**, so it is Live-faithful and
**left as-is**.

**Deferred to F1/F2:** the port's refactor also **dropped** two upstream behaviors —
`awardSpaceFactionPoints` for overt players and the imperial/rebel faction-reward handling (upstream
`:89-94` of the function region), and lowered the damage threshold `0.5f → 1`. These do not affect the
two formulas audited here, but the faction-point drop may impact GCW point accrual (§1.3 "space PvE GCW
point per tier"). **Re-adding requires the FactionManager call to compile/boot-verify; flag F2** rather
than re-introduce blind in a data-only phase.

**Correction applied:** none in F0 (formula already matches Live; dropped GCW award deferred to F2).

---

## Item 5 — Even-level cap/booster rule + component stat semantics — **MATCH** [B]

**Live spec (§3.3-3.4 [EP]):** components are **cert level 1–10**; capacitor = `maxEnergy` +
`rechargeRate`; booster = energy + consumption rate + recharge rate; reactor powers all; shield =
front/back HP + **per-side** recharge; "even levels (2,4,6,8,10) are where it's at" for
capacitors/boosters.

**Port actual** (`MMOCoreORB/bin/scripts/object/tangible/ship/components/`):
- Cert levels 1–10 via `reverseEngineeringLevel` + per-level `modifiers` fractions on every component
  template (e.g. `weapon_capacitor/cap_corellian_cruiser_grade_cap1.lua`,
  `booster/booster_test.lua`). (#12)
- **Capacitor** carries exactly `maxEnergy` + `rechargeRate` (§3.4) — MATCH.
- **Booster** carries `maximumEnergy` + `consumptionRate` + `rechargeRate` (+`acceleration`,`maxSpeed`)
  (§3.4) — MATCH.
- **Reactor-powers-all + per-side shields + capacitor energy/recharge economy** implemented in
  `ShipObjectImplementation.cpp` energy tick (`:809` reactor energy, `:871-894` front/rear shield,
  `:904-910` capacitor maxEnergy+rechargeRate). Matches §3.2/§3.4 (#11,#13).
- **Even-level rule:** this is an emergent *loot/RE drop-distribution* meta ("best RE results come from
  even-level drops"), **not a hardcoded engine gate**. The port correctly models cert levels 1–10 +
  per-level modifiers and does **not** hardcode an even-level rule — which is the faithful
  implementation; the even-level breakpoint is a property of the loot/RE roll tables, not of the
  component stat code. (#12,#17)

**Verdict: MATCH.** Component stat semantics and cert-level structure match Live (= SWGEmu RE [B]).

**Correction applied:** none.

---

## Item 6 — Chassis data spot-check (8 chassis) — **MATCH 8/8** [A primary, B for maneuver]

Extracted `ship_chassis.iff` [A] (313 rows, read via `iff_datatable.py`) supplies the **weapon-slot
layout** (= guns + missile + turret hardpoints). Mass and Spdx/accel/maneuver caps are SWGEmu RE'd [B]
in the in-repo `player_*.lua` chassis templates (each verified identical to upstream where checked).
"Guns" in the §3.1 [COMM] table = blaster hardpoints; total weapon slots = guns + missiles + turrets.

| Chassis | §3.1 Live (mass / guns+msl / Spdx, Acc/Dec, Yaw/Pitch/Roll) | Port (extracted slots [A] + template [B]) | Verdict |
|---|---|---|---|
| X-Wing | ~100k / 3g+1m (4 slots) / 0.95, 25/30, 200/300/150* | mass 100000; `player_xwing` 4 wpn slots [A]; Spdx 0.95, 25/30, Yaw200 Pitch300 Roll150 [B] | **MATCH** |
| A-Wing | ~65k / 1g+1m (2 slots) / 1.0, 40/60, 250/250/125 | mass 65000; `player_awing` 2 wpn slots [A]; Spdx 1.0, 40/60, 250/250/125 [B] | **MATCH** |
| B-Wing | ~240k / 4g+2m (6 slots) / 0.85, 10/15, (Y/P 50)/25 | mass 240000; `player_bwing` 6 wpn slots [A]; Spdx 0.85, 10/15, Yaw35 Pitch35 Roll25 [B] | **MATCH** (slots/mass/Spdx exact; Yaw/Pitch = SWGEmu-RE 35, see note) |
| Y-Wing | ~150k / 1g+1m+turret (3 slots) / 0.85, 15/25, 80/80/40 | mass 150000; `player_ywing` 3 wpn slots [A]; Spdx 0.85, 15/25, 80/80/40 [B] | **MATCH** |
| TIE-Advanced | ~65k / 2g+1m (3 slots) / 1.0, 40/60, 300/300/150 | mass 65000; `player_tieadvanced` 3 wpn slots [A]; Spdx 1.0, 40/60, 300/300/150 [B] | **MATCH** |
| TIE-Fighter (TIE-F) | ~20k / 1g+1m (2 slots) / 0.98, 40/50, 500/500/250 | mass 20000; `player_tiefighter` 2 wpn slots [A]; Spdx 0.98, 40/50, 500/500/250 [B] | **MATCH** |
| TIE-Bomber | ~190k / 2g+2m (4 slots) / 0.85, 10/15, 50/50/25 | `player_tiebomber` 4 wpn slots [A] | **MATCH** (slot count) |
| TIE-Interceptor | ~50k / 1g+1m (2 slots) / 1.0, 40/60, 300/300/150 | mass 50000; `player_tieinterceptor` 2 wpn slots [A]; Spdx 1.0, 40/60, 300/300/150 [B] | **MATCH** |

\* The scrapbook lists X-Wing as `300/200/150`; mapping to engine fields the port stores Pitch 300 /
Yaw 200 / Roll 150 — same three values, consistent.

**B-Wing Yaw/Pitch note (provenance decision):** scrapbook §3.1 [COMM] lists `50/50/25`; the port and
**upstream `swgemu/Core3`** both hold `35/35/25` (`player_bwing.lua`, verified byte-identical to
upstream). Per F0 precedence the **SWGEmu-RE value [B] outranks the player-measured community number
[COMM]**, and the directive forbids inventing balance. **The port value is the authoritative Live value;
no correction applied** (changing 35→50 would move *away* from the canonical reconstruction).

**Verdict: MATCH 8/8.** Every spot-checked chassis's slot layout matches the extracted SOE table [A];
mass and maneuver caps match the SWGEmu RE'd templates [B].

**Correction applied:** none.

---

## Death-spiral finding (called out per deliverable)

The interceptor death-spiral **is present and is Live-faithful**, implemented exactly as upstream
SWGEmu Core3 (`SpaceCombatManager.cpp` is byte-identical to `swgemu/Core3`). A shot resolves
shield→armor→one-random-component→chassis, and when a low-HP component is destroyed its absorption is
removed and remaining/subsequent damage reaches the hull faster — the structural cause of the spiral.
The port's mechanism excludes an already-destroyed slot from re-selection (vs the scrapbook [COMM]
"dead slot stays in pool") but yields the same emergent behavior; this is the canonical RE'd model and
is the accepted Live standard (corpus §B). Per the directive it was **explicitly NOT "fixed"** — it is
intended period-correct behavior. **No change made.**

---

## Items needing a live boot (F1/F2)

1. **F1** — Per-tier pilot skill gating (T1/T2/T4 = XP+mission vs **T3/Master = missions-only**) and
   skillpoint-free cost: defined in the binary `skills.iff` (client TRE), not in-repo; verify on boot.
2. **F2** — `disseminateSpaceExperience` dropped the upstream `awardSpaceFactionPoints` (overt GCW
   points) + imperial/rebel faction-reward handling; confirm GCW-point accrual on boot and decide
   whether to restore (compile/boot needed).
3. **F3 (content, not boot)** — squadron Master mission should be the Corvette-in-Kessel kill (§2.4),
   currently a generic destroy quest.

## Provenance ledger

| Audit item | Authoritative source used | Tier |
|---|---|---|
| 1 Damage model | `SpaceCombatManager.cpp` == upstream Core3; scrapbook §3.5 | B (governs) / [COMM] |
| 2 Speed/maneuver | `ShipObjectImplementation.cpp` == upstream; scrapbook §3.1/§3.4 | B / [COMM] |
| 3 Cert/squadrons | in-repo squadron screenplays; scrapbook §2.2/§2.3 [D/EP]; skills.iff (TRE) for tier-XP | D (struct) / deferred |
| 4 Crew XP + master bonus | `PlayerManagerImplementation.cpp:1534-1652`; scrapbook §1.3/§2.5 [D] | D |
| 5 Even-level / component semantics | component `.lua` templates == SWGEmu RE; scrapbook §3.3/§3.4 | B / [EP] |
| 6 Chassis spot-check | extracted `ship_chassis.iff` [A]; `player_*.lua` == upstream [B]; scrapbook §3.1 | A / B / [COMM] |
