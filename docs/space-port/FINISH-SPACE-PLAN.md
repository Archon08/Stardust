# Finish Space — Live-Faithful Completion Plan

Goal: space **works like Live** (Pre-CU/JTL era), minus the odd bug to catch later. Hard rule:
**use original SWG data; invent no balance.** All code/plans written to the GitHub repo only.

## Inputs already in hand
- Port compiles + links green (P0-P6, tag `p2p3-green`); content dropped + registered (P4); Phase 5
  reconciliation done; Phase 6 test/sim harness exists.
- **Authentic data corpus** (docs/space-port/fidelity/SWG-DATA-CORPUS.md):
  (A) 386 extracted original SOE space datatables; (B) 995 SWGEmu RE'd component stat templates
  (in-repo); (C) Live Stardust SQL snapshot (Drive, validation); (D) **scrapbook Live-reference**
  (LIVE-REFERENCE-SCRAPBOOK.md — JTL mechanics/pilot certs/ship data/squadrons from original
  forums+publish notes).
- Extraction/analysis tooling: tools/fidelity/{tre_read,iff_datatable,deathspiral}.py +
  .github/workflows/{component-stats,space-tests,space-port-build,g1-smoke,space-lint}.yml.

## Division of labor (the one constraint that shapes everything)
I write **all code, content, harnesses, plans** to the repo and validate everything CI can validate
(compile/link/lint/unit-tests/sim) in the cloud. The **only** step that needs you is the **live boot
with real client TREs** (I can't boot locally per your rule; assets are too large for cloud CI). The
plan makes that boot **self-driving**: one launch runs a scripted battery and emits a full defect
list — you never iterate manually.

---

## Phase F0 — Fidelity Lock (data is provably Live)
Make the space DATA match original SWG before chasing runtime behavior.
1. **Audit** port in-repo space values vs (A) extracted datatables, (B) SWGEmu RE'd stats, and the
   (D) scrapbook Live-reference checklist. Tooling: extend tools/fidelity/ with an audit script that
   diffs in-repo datatables/component-lua against the authentic corpus.
2. **Correct** any divergence to the Live value (no balance choices — just match original). Where the
   three sources disagree, precedence = official publish notes (D) > extracted SOE datatable (A) >
   SWGEmu RE (B); record provenance per correction.
3. Gate **F0**: every space datatable/component value either matches the authentic corpus or has a
   documented provenance note. Deliverable: FIDELITY-AUDIT.md + corrections committed.

## Phase F1 — Boot to Green (runtime truth baseline)  [needs your 1 boot]
1. **Self-driving boot harness** (committed): a script that boots core3 with JTLEnabled=true + the
   MTG/Stardust+JTL TRE set + DB, then drives a scripted battery (the SCENARIO-MATRIX) — launch into
   each of the 10 zones, spawn check, fly, fire, accept+complete one mission per type — and dumps a
   structured pass/fail + error log. One run = the whole defect list.
2. **Runbook** (committed, refresh of OPERATOR-RUNBOOK.md): exact build + TRE path + DB + run steps.
3. You run it once; send the log back. Gate **F1**: server boots, all 10 zones load, NPC ships spawn,
   no fatal errors to first-playable. Deliverable: defect list (the real Phase-0 baseline).

## Phase F2 — Runtime defect sweep (iterate to clean)
Work F1's defect list in the repo, re-boot via the harness, repeat. Targets, in order:
1. Zone load + spawn correctness (regions/spawners/loadouts across all 10 zones).
2. Player flow: LaunchIntoSpace -> fly -> fire -> hit resolution -> land -> store ship.
3. Space combat: damage model, component damage, destruction, loot.
4. NPC AI: btspace pilot behavior, squadron formations, aggro/threat.
Gate **F2**: a pilot can launch, fly, fight NPCs, and complete a basic mission in every zone.

## Phase F3 — Content completion (Live-faithful)
1. **Author the 7 missing squadron chains** (only Corsec/Rsf are wired upstream) using the scrapbook
   squadron/themepark structure (D) + existing 2 as templates. Live-faithful mission flow + rewards.
2. **Pilot certification end-to-end**: the 3 faction trees (Imperial Navy/Rebel/Freelance) + ace
   progression, gating the themepark, granting certs as Live did (structure from D).
3. **Themepark rewards** at Live values (certs + ship parts as Live, not credit-inflation).
Gate **F3**: all 9 squadron chains completable; pilot tiers gate content as Live.

## Phase F4 — Systems completion
1. **awardSpaceFactionPoints** + space faction system (deferred in P5 — needs the space-faction
   subsystem ported from upstream).
2. **Multi-crew POB**: pilot + gunner/turret stations + interior occupancy sync (the hardest
   correctness item; assume real engineering).
3. **Hyperspace routing** between all zones; **spaceport launch/land** at every zone.
4. **Component crafting/loot**: shipwright schematics + space loot resolve to Live-stat components.
Gate **F4**: feature parity with Live JTL systems.

## Phase F5 — Live-faithfulness validation + polish
1. Run the full SCENARIO-MATRIX; **validate behavior against the scrapbook Live-reference** (D) —
   pilot pacing, mission rewards, combat feel benchmarks documented by Live players.
2. Performance: octree/broadcast tuning under multi-ship dogfight load (range consts 2048/8192/32768).
3. Ground-space seam: persistence, faction, economy crossover.
Gate **F5 (DONE)**: space is workable like Live; residual = the "few bugs to catch."

---

## What's automated (me, in-repo, cloud-validated) vs you
- Me: F0 fully; F2/F3/F4 code+content+fixes; F1/F5 harnesses+matrices; all CI validation.
- You: run the F1 self-driving boot (once per defect-sweep cycle) and send logs. That's the only
  manual loop, and it's batched (one boot = many defects), not per-bug.

## Sequencing
F0 now (data-only, no boot). F1 harness+runbook now (ready for your boot). F2 iterates on boot logs.
F3 can run in parallel with F2 (content has no compile dep). F4 after F2 green. F5 last.

## Calibration note
The F0 audit precedence and F3 squadron/cert specifics get finalized from
LIVE-REFERENCE-SCRAPBOOK.md the moment the scrapbook mining completes; this plan is the skeleton,
that doc supplies the Live numbers/structure.
