# Stardust Space Port — Master Plan

**Goal:** Bring Jump to Lightspeed (JTL) space to the Stardust server by porting the modern
swgemu/Core3 space subsystem **into** the Stardust `unstable` tree (2019 vintage).

**Basis:** June 8, 2025 deep investigation of swgemu/Core3 + swgemu/engine3 ("the receipts"):
JTL upstream is ~80-90% structurally complete — all four 2019 architectural blockers are solved,
only 3 real TODOs in ~29k lines of space C++. What remains upstream is verification and tuning,
not architecture. What remains *for Stardust* is getting that subsystem into this tree.

## Decision record

| Decision | Choice | Date |
|---|---|---|
| Strategy | **Port space INTO the 2019 Stardust tree** (vs rebase onto modern Core3) | 2026-06-11 |
| Workspace | This GitHub repo only — no local working copies | 2026-06-11 |
| Fallback gate | End of Phase 1: if engine-uplift fallout exceeds budget, pivot to rebase-onto-Core3 with the Stardust delta re-applied (see 02, Phase 1 gate) | 2026-06-11 |

## Hard facts that shape everything (full evidence in 01-receipts.md)

1. **This fork predates all JTL work.** Last commit 2019-06-30. No Zone split, no octree,
   `objects/ship/` is the vanilla 3-file stub (zero Stardust customization there — replace wholesale).
2. **The octree lives in engine3, not Core3.** Stardust pins no engine (external 2019 PublicEngine
   via `find_package(Engine3)`); modern Core3 pins engine3 as a submodule at `7012c03`. The port
   therefore REQUIRES an engine3 uplift + toolchain jump (C++11 → C++17, modern idlc). This is the
   single largest risk and is isolated into its own phase with a go/no-go gate.
3. **The space subsystem is cleanly clustered.** ~272 C++ files in dedicated dirs
   (`objects/ship/**`, `managers/{ship,space,spacecollision,spacecombat}`, `packets/{jtl,ship}`)
   plus ~11 shared files carrying ~600 space-referencing lines. The invasive surface is small and known.
4. **Stardust's customizations collide where space reads player state.** Mind-pool removal,
   combat rebalance, Armorweaver, badge-Jedi live in CreatureObject/CombatManager/PlayerManager —
   the same files modern pilot/space-combat code touches. This reconciliation is its own phase.
5. **JTL master switch defaults OFF upstream** (`Core3.JTL.JTLEnabled = false`). Even upstream treats
   this as soft-launch. We inherit that posture: space lands dark, gets verified, then gets enabled.

## Document map

| Doc | Contents |
|---|---|
| [01-receipts.md](01-receipts.md) | Evidence base: Core3-2026 space inventory, the four solved 2019 blockers, Stardust-2019 audit |
| [02-port-plan.md](02-port-plan.md) | The phased port (Phases 0-6) with file-level work items and gates |
| [03-collision-map.md](03-collision-map.md) | Shared-file collision table + Stardust custom-delta hotspots |
| [04-verification.md](04-verification.md) | Instrumentation, QA-at-scale, balance simulation, fidelity tuning |
| [05-risks-open-questions.md](05-risks-open-questions.md) | Risk register, open questions, AGPL + client-asset notes |

## Effort calibration

Estimates are calibrated to an AI-accelerated generate-test-iterate loop (agent swarm + fast build
cycles), NOT volunteer-team pace. Phases are sized in iteration-days, not calendar months. The two
genuinely hard engineering buckets — engine uplift (Phase 1) and POB multi-crew sync (Phase 6) —
are flagged as the least compressible.