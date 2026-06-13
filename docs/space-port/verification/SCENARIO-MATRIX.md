# Space-Port Verification Scenario Matrix (Phase 6.A live-verification spec)

This is the **spec** for live space verification: a concrete checklist a headless-client
harness (or an operator with a client) executes against a running server. It is an
ENGINEERING deliverable; **execution requires a client + live server** and therefore is
not part of the headless CI gate (that gate is `space-tests.yml` — pure-math + balance
sim). Everything here is what CI cannot reach.

Dimensions: **10 zones × 9 squadron chains × mission-type × ship-class.**

## Zones (10) — confirmed from `bin/scripts/managers/space/space_manager.lua`

| # | Zone id | Type |
|---|---|---|
| 1 | `space_corellia` | open sector |
| 2 | `space_dantooine` | open sector |
| 3 | `space_dathomir` | open sector |
| 4 | `space_endor` | open sector |
| 5 | `space_lok` | open sector |
| 6 | `space_naboo` | open sector |
| 7 | `space_tatooine` | open sector |
| 8 | `space_yavin4` | open sector |
| 9 | `space_light1` | instanced / tutorial (light) |
| 10 | `space_heavy1` | instanced / tutorial (heavy) |

## Squadron chains (9) — pilot certification factions

Grounded in the space loot/faction families and the pilot-cert squadron screenplays
(Corsec/Rsf pilot tiers referenced by Phase 5). Each chain is a tiered progression
(tier 1→5 where applicable).

| # | Squadron chain | Alignment |
|---|---|---|
| 1 | Imperial Navy | Imperial |
| 2 | Rebel Alliance (RSF) | Rebel |
| 3 | CorSec | Neutral/Corellia |
| 4 | Black Sun | Pirate/criminal |
| 5 | Hutt | Criminal |
| 6 | Nym's Pirates | Pirate |
| 7 | Civilian / Freelance | Neutral |
| 8 | Merchant escort | Neutral/trade |
| 9 | Generic pirate/raider | Hostile |

## Mission types

| Code | Mission type | Pass signal |
|---|---|---|
| M1 | Destroy (kill N enemy ships) | kill count credited, mission completes, reward granted |
| M2 | Patrol / waypoint | all waypoints registered, completion fires |
| M3 | Escort (protect convoy) | escortee survives to destination |
| M4 | Duty/assembly (wave defense — `waveAttack.lua`/`spaceStations.lua`) | all waves cleared |
| M5 | Inspection / survey | scan/inspect interaction resolves |
| M6 | Deliver / courier | cargo delivered at destination zone |

## Ship classes

| Code | Ship class | Notes |
|---|---|---|
| S1 | Light fighter / interceptor | maneuver build — death-spiral subject |
| S2 | Heavy fighter | balanced |
| S3 | Gunship | DPS/tank |
| S4 | Bomber | anti-capital |
| S5 | POB / multipassenger | crewed; subsystem targeting |
| S6 | Capital / AI capital ship | `CapitalShipObject` — turret/section logic |

## Per-cell checklist (what the harness asserts)

For each `(zone, squadron-chain, mission-type, ship-class)` cell the harness verifies:

- [ ] **Zone entry**: player/ship transfers into the space zone; `isSpaceZone()` true;
      launch point matches `space_manager.lua` `jtlLaunchPoint`.
- [ ] **Active areas**: ship registers in the expected `SpaceActiveArea`(s) on entry and
      deregisters on exit/teleport (the behavior `SpaceZoneTest` checks headlessly is
      reproduced live with real spawns).
- [ ] **Spawn / squadron**: the squadron-chain mission spawns the correct faction/tier
      group (cross-check vs `loot/groups/space/faction/space_<faction>_tier<N>.lua`).
- [ ] **Combat resolves**: weapons fire, `SpaceCombatManager` damage applies through
      shield → armor → chassis; target dies; no desync between server and client HP.
- [ ] **Transform/interp**: ship movement is smooth (no rubber-banding); server
      `ShipObjectTransform` updates match client-side prediction within
      `POSITION_EPSILON` / `ROTATION_EPSILON`.
- [ ] **Mission lifecycle**: mission accepts, progresses, completes, and pays out for the
      mission-type under test.
- [ ] **Pilot certification**: required pilot tier/squadron gates the mission (ties to the
      Phase 5 pilot-cert Lua bindings — verify a too-low tier is refused).
- [ ] **Cleanup**: leaving the zone / mission abort tears down spawns and active areas
      without leaking objects.

## Coverage scope & prioritization

Full enumeration is `10 × 9 × 6 × 6 = 3240` cells. That is the exhaustive ceiling, not the
required run. Recommended tiered execution:

1. **Smoke (must-pass before any space release):** each zone (10) entered once with S1 and
   S3 on M1 — 20 cells. Proves zone load + active areas + combat + death across all zones.
2. **Squadron coverage:** each squadron chain (9) run once on its native faction zone with
   its canonical ship class on M1/M4 — 18 cells. Proves spawn tables + pilot cert.
3. **Mission-type coverage:** each mission type (6) run once in one open zone with S2 — 6
   cells. Proves mission lifecycle plumbing.
4. **Class coverage:** each ship class (6) flown once on M1 in `space_heavy1` — 6 cells.
   Proves per-class transform/combat (esp. S5 POB and S6 capital subsystem logic).
5. **Full matrix:** run only when curve-tuning (post-6.B) or chasing a zone-specific bug.

Total prioritized smoke+coverage = **50 cells** — the practical live-verification gate.

## Headless-client harness requirements (to actually execute this)

- A scriptable/headless SWG client able to authenticate, launch to space, target, fire,
  and read HUD/HP state (or a server-side scripted bot that drives a `ShipObject`).
- A seeded test server with TRE datatables loaded (so chassis/components are real).
- A results sink: per-cell PASS/FAIL + telemetry (TTK, desync max, leaked-object count).

Until that harness exists, this matrix is executed manually by an operator and the results
recorded against the checklist above.
