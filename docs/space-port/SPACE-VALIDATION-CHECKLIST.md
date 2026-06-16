# Stardust Space — Gameplay Validation Checklist (post-uplift)

Server state: `stardust-space` branch **compiles, links, boots clean, lint-green**. Headless boot
verified content-load + all 10 space zones deploy with 0 faults. This checklist covers what a headless
boot **cannot** verify — the in-client experience. Run on a real client against a server built from
`stardust-space`.

## How to boot (helper)
1. Build core3 from `stardust-space` (recipe: `.github/workflows/space-port-build.yml`) OR use a
   `core3-binary-*` CI artifact.
2. DB: load `MMOCoreORB/sql/swgemu.sql` + `datatables.sql` into a `swgemu` database (user `swgemu`/`123456`).
3. TREs: point `bin/conf/config.lua` `TrePath` at the 26 Stardust/MTG TREs.
4. `cd bin && ./core3`. (For diagnostics, run `tools/boot/space-boot-capture.sh` → sends back one bundle.)

## A. Pilot certification (all 9 squadrons) — the F3 headline
For each faction line, visit the recruiter, train, and confirm tier progression via `incrementPilotTier`:
- [ ] **Rebel**: Havoc (Kreezo/Aqzow/Viopa), Vortex (V3-FX), Crimson Phoenix (Da'la Socuna)
- [ ] **Imperial**: Storm (Akal Colzet), Inquisition (Barn Sinkko), Black Epsilon (Hakassha Sireen)
- [ ] **Neutral**: Corsec (Rhea/Ramna/etc.), RSF (Dinge/Dulios/etc.), Smuggler (Dravis)
- [ ] Novice grant works; T1→T2→T3 boxes train (XP gate via `fulfillsSkillPrerequisitesAndXp`).
- [ ] `setSquadronType`/`isSquadronType` correct (joining a squadron sticks; switching gated correctly).
- [ ] Master pilot reachable (T3/master = missions-only progression, per Live).
- [ ] **Quest journal UI** populates for space missions (JournalQuest natives now wired) — confirm a
      received space mission appears in the client Quest Journal window and completes/clears there.

## B. Squadron recruiter NPCs (flag any wrong appearance)
Duplicate `overwriting mobile` registrations exist for some space NPCs (benign last-wins; pre-existing).
Confirm each recruiter NPC appears correctly; flag any that look wrong:
- [ ] da_la_socuna, adwan_turoldine, zo_ssa, and the squadron recruiters render with correct
      appearance/faction/conversation.

## C. Flight + combat (like-live feel)
- [ ] Launch from a spaceport / land; POB multi-crew: enter a station, **Leave Station** works
      (regression fixed), pilot/gunner/operator roles.
- [ ] Ship speed = engine × 10 × Spdx feels right; component damage model (shield→armor→random
      component→chassis spillover) — the interceptor **death-spiral is authentic Live; do NOT "fix" it**.
- [ ] Hyperspace routing between the 10 space zones.

## D. Kessel Master encounter (Gaps 3 & 4)
- [ ] Stage 1 (`master_*_1`): destroy 30 enemy fighters in-zone.
- [ ] Stage 2 (`master_*_2`): the enemy Corvette + 2 gunboat escorts spawn at the exit geometry.
- [ ] **Gap 3 — dwell**: if you do NOT destroy the corvette within ~45 min, it **hyperspaces out**
      (corvette + escorts despawn; you get the "jumped to hyperspace" message); re-entering Kessel
      respawns it. (Constants: `KESSEL_CORVETTE_DWELL_MS` etc., scrapbook-sourced.)
- [ ] **Gap 4 — reward**: on completing `master_*_2` you receive the **"ace pilot" necklace** wearable
      (Imperial pilots → empire variant, Rebel → rebel; gender-correct m/f) + 25000 credits.
      → **FLAG if**: the necklace doesn't appear in inventory, or renders wrong. Known cosmetic gaps:
      Wookiee/Ithorian get the base mesh (not their `wke`/`ith` variant); the Badge + faction Helmet +
      autoreward mail body are not yet wired (documented follow-ups).

## E. Known NON-space pre-existing bugs (out of scope — fix only if desired)
- [ ] `mobile/dathomir/axkva_min.lua:38` syntax error (Nightsister boss fails to load) — pre-existing.
- [ ] 7 Nightsister clothing draft-schematics reference templates not in the TRE set — pre-existing.

## What "fully working like-live" means here
The **server systems** (all squadrons, certification, JournalQuest missions, 10 space zones, flight,
combat, Kessel) load and run clean. This checklist is the human confirmation of the **in-client feel**.
Report anything that diverges from Live Pre-CU/JTL behavior — except the death-spiral, which is authentic.
