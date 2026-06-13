# 04 — Verification, Tuning & QA Program

This is the program that closes the gap between "structurally complete" and "plays like Live."
It maps the June 8 phased completion analysis onto the Stardust port (Phase 6 of 02-port-plan.md).
Build the harness EARLY (during port Phases 2-3) so verification starts the day content lands.

## A. Instrumentation & Truth Baseline

**Purpose:** convert "we think it works" into a defect list. This was impossible at scale for a
volunteer team in 2019; it is the single highest-leverage automation investment.

- Dev server with `SpaceZonesEnabled` (all 10) + `JTLEnabled=true`, isolated from live data
- **Headless-client behavioral harness** scripting a client through every subsystem:
  launch → fly → fight → mission-accept → mission-complete → land, per zone
- Scenario matrix (agent-generated): 10 zones x 9 squadron chains x mission taxonomy
  (Assassinate, Battle, Delivery, Destroy, Duty x5, Escort, Inspect, Patrol, Recovery, Rescue,
  Surprise Attack, Survival) x ship classes
- Output: pass/fail map per (zone, chain, mission-type, ship-class) tuple + server log sweep
- **This answers the order-of-magnitude question:** are the squadron chains authored-and-working
  or authored-but-untested? No effort commitments before this runs.

## B. Fidelity Reconstruction (clean-room)

**Purpose:** component stat curves and flight constants that feel like Live.

- Source: client datatable values (`ship_chassis.iff`, component datatables) — **data, not SOE
  server code; clean-room-safe**
- Method: Proposer/Verifier/Arbiter trio — Proposer drafts formula from observed stat points;
  Verifier checks against archived wiki/parse data (Pre-CU Scrapbook is a local source);
  Arbiter emits a provenance record per constant
- Every tuned constant gets a provenance entry (legally-defensible clean-room record — this is
  the distinction the SWGEmu community cares about)
- Stardust wrinkle: where Live values conflict with Stardust's economy/combat rebalance,
  fidelity records the Live value and a Stardust override is a SEPARATE, explicit decision

## C. Balance via Simulation

**Purpose:** quantify and fix balance before a human flies.

- Monte Carlo headless engagements across ship/component permutations (thousands of runs)
- Explicit target: **the interceptor death-spiral** — surface the exact stat breakpoints that
  cause it; auto-tune curves within fidelity bounds from §B
- Regression: balance suite re-runs on every stat-curve change

## D. Multi-Crew POB Correctness

**Purpose:** the hardest genuine engineering left anywhere in JTL.

- Scope: `PobShipObject` + `ShipStationContainerComponent` — synchronized multi-player flight:
  pilot + gunner/turret stations + interior cell occupancy + (later) interior persistence
- Most likely subsystem to be partial upstream; assume real engineering, not content
- Least AI-acceleratable; mitigation = heavy automated regression from §A harness (multi-client
  scenarios: board, man turret, fire while pilot maneuvers, disembark, server-crash recovery)

## E. Content QA Sweep & Themepark

- Walk all 9 squadron chains end-to-end (Havoc, Crimson Phoenix, Vortex / Black Epsilon,
  Inquisition, Storm / CorSec, RSF, Smuggler) — agent-driven playthroughs, humans adjudicate
  fidelity against Live behavior archives (Pre-CU Scrapbook at ForgeVault is a primary reference)
- Dynamic mission generators per type per zone
- Fill conversation/reward gaps found; verify certification gating (pilot tiers gate themepark
  progression) — already flagged as needing confirmation upstream
- AI reaction chats: one of the 3 actual upstream TODOs — behavior-tree completeness for
  believable dogfighting

## F. Performance & Seam Polish

- Octree tuning + broadcast budget under load: many fast AI ships dogfighting simultaneously
  (range constants: close 2048 / capital 8192 / station 32768 — tune against real densities)
- Every spaceport launch/land pair on all 10 zones; hyperspace routing between all zones
- **Stardust-specific decision:** do the 6 custom planets (chandrila, coruscant, jakku, kaas,
  moraband, hutta) get space sectors? Out of scope for the port; record as content roadmap item
- Content-director reconciliation loop: declare desired sector state (spawns, patrols, mission
  density), reconciler holds it true; DirectorManager already touches spaceZone

## G. Launch criteria (Gate G6)

1. §A matrix: 100% boot/launch/land, squadron chains completable, zero crash defects open
2. §C: no dominant-strategy breakpoints at tier 1-2; interceptor spiral resolved or consciously deferred
3. §D: POB minimum bar — pilot+gunner stable for a full mission; interior persistence may be staged
4. Ground regression: zero deltas vs Phase 0 baseline on live-config server
5. Rollout: `JTLEnabled=true` with zone subset if desired (config supports per-zone enable list)