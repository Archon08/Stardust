# 05 — Risks, Open Questions, Compliance

## Risk register

| # | Risk | Phase | Likelihood | Impact | Mitigation |
|---|---|---|---|---|---|
| R1 | engine3 2019→2026 API drift fallout exceeds budget | 1 | Medium-High | Critical path | Do it first; hard GO/NO-GO gate G1; strategy-neutral Phase 0 delta inventory enables clean pivot to rebase |
| R2 | Mind-pool removal breaks pilot/space-combat attribute reads at runtime (not compile time) | 5 | High | High | Dedicated audit 5.1; harness scenarios exercise every pilot command cost |
| R3 | DirectorManager three-way merge regresses Stardust custom events | 5 | Medium | High | Merge session + custom-screenplay regression in smoke harness (battle_of_endor scripted check) |
| R4 | MTG/stardust TREs override or conflict with JTL client assets | 4 | Medium | Medium | Asset audit 4.6 against actual client manifest before any live enable |
| R5 | idlc regeneration silently drops Stardust custom IDL fields | 1, 3 | Medium | High | Never hand-merge generated headers; diff regenerated output against Phase 0 baseline symbols |
| R6 | POB multi-crew sync is more broken upstream than estimated | 6 | Medium | Medium | Staged launch criteria (G6.3) — POB can ship after fighters |
| R7 | Squadron chains authored-but-untested → Phase 6 balloons | 6 | Unknown | Order-of-magnitude on 6.E | §A harness answers it before commitments; code density suggests working-but-unverified |
| R8 | Berkeley DB migration corrupts existing player data | 5 | Low | Critical | Schema snapshot 0.4; migration tested on copy; never run on live first |
| R9 | 6 custom ground zones break in Zone→GroundZone migration | 2 | Low-Medium | High (Stardust identity) | Explicit 2.2 work item; per-zone smoke test in gate G2 |
| R10 | Upstream Core3 keeps moving during the port | all | Certain | Low-Medium | Pin the port to a fixed Core3 snapshot SHA at Phase 0; absorb later upstream fixes as a post-G6 sync |

## Open questions (answers change the plan)

1. **Squadron chains: working or just authored?** (R7) — answered by the instrumentation harness,
   nothing else. No timeline commitments before this.
2. **Which Core3 snapshot to pin?** Recommend latest `unstable` SHA at Phase 0 start; record in
   the decision log.
3. **2019 engine3/PublicEngine availability** for the Phase 0 baseline build — ZonamaDev-era VM
   or container recreation; if unobtainable, baseline becomes compile-audit-only and the smoke
   harness starts at G1 instead.
4. **Space sectors for the 6 custom planets** — content roadmap, post-launch.
5. **Live rollout shape** — all 10 zones at once vs staged subset (config supports either).
6. **Client distribution** — does the Stardust launcher/manifest need updating to guarantee JTL
   client assets present (base-game JTL .iffs) alongside MTG patches?

## Compliance & licensing

- Core3, engine3, Stardust are all **AGPL-3.0**. Porting AGPL→AGPL is clean. AGPL obligations:
  if the server runs publicly, the modified source must be available to users — this repo being
  public satisfies that as long as the running code matches a published tree.
- **Clean-room discipline:** fidelity constants come from client DATA (datatables) and public
  archives only — never from leaked SOE server code. The Proposer/Verifier/Arbiter provenance
  records (04 §B) exist to prove this.
- ModTheGalaxy TRE dependency: client-asset licensing is MTG's; no MTG assets enter this repo.

## Out of scope (recorded so nobody re-litigates)

- UE5/Starborne integration — this port is a Core3-native effort; Starborne consumes it as
  reference data/semantics (space is V3 in the Starborne roadmap), not as a runtime dependency.
- New space content beyond Live parity (custom sectors, new ships) — post-G6 roadmap.
- NGE-era space features — Pre-CU/JTL-launch parity only.