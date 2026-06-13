# JTL / Space — Live (Pre-CU/JTL-era) Reference, mined from Biophilia's SWG Pre-CU Scrapbook v5.1

**Purpose.** Authoritative LIVE (Pre-CU / Jump to Lightspeed era, ~2004–2006) reference for the SWG
space subsystem, extracted from an archived mirror of the *official* Star Wars Galaxies website and
player forums. Use this to validate that the Stardust space port's values and behavior are
**Live-faithful**. The directive is: **use original SWG data; do not invent balance.** Where a value is
not documented in the corpus, this doc says so explicitly rather than guessing.

**Clean-room note.** This is a structured *summary of facts* extracted from archived public official
update notes and community guides, used as a behavioral/values reference. No raw copyrighted page dumps
are reproduced wholesale.

**Corpus.** "Biophilia's SWG Pre-CU Scrapbook v5.1" — captured pages under
`.../Starborne/PreCUScrapbookv5.1/Biophilia's SWG PreCU Scrapbook v5.1/data/<timestamp>/`.
Each cited path below is **relative to that `data/` directory**. Captures are dated Jan–Feb 2007 but
mirror official notes and forum guides authored 2004–2006.

**Provenance / confidence tiers used below:**
- **[OFFICIAL]** = from SOE/SWG official Publish/Update Note Archive pages.
- **[OFFICIAL-EP]** = forum guide explicitly marked "(OFFICIAL)" or authored by the pilot correspondent.
- **[COMMUNITY]** = well-regarded player-authored guide; values are player-measured, corroborative but
  not first-party. Most concrete numbers in JTL come only from this tier (SOE never published raw stat
  tables), so these are the de-facto authority and align with SWGEmu's reverse-engineered values.

---

## 1. JTL mechanics — from official publish notes

### 1.1 Expansion launch (Publish 11) — [OFFICIAL]
Source: `20070126114830/content_002.html` (Publish Archive 2004).

- **Jump to Lightspeed shipped as Publish 11, in two parts:** Part 1 on **October 21, 2004** (pushed
  most of the code), Part 2 on **October 26, 2004** (final files that *enabled* JTL for owners).
- JTL access is gated on owning the expansion + entering the key. Pre-order rewards: **Flash Speeder**
  (pre-order) and the **Sorosuub 3000 Luxury Yacht** (also a 180-day veteran reward — see §3).
- Ground professions received **Pre-JTL space-crafting schematics** at launch:
  - Architect → "Space crafting station"; Artisan → "Space crafting tool".
  - Droid Engineer → "Space crafting station module", **Flight Computers**, **Unprogrammed Droid Command
    Modules**.
- **Pilot, Pilot Recruiter, and Shipwright NPCs** were placed throughout the world and **"will only
  respond if you are JtL enabled."** Many NPCs added near starports referencing Pilots/Shipwrights.

### 1.2 Corellian Corvette space themepark — [OFFICIAL]
Source: `20070126114830/content_002.html`.

- "Corellian Corvette adventures … Players can now play through **nine possible quests** to gain access
  to SWG's space-themed adventure. This **instanced dungeon** can be played … alone."
- Faction framing: "**Imperial officers, Rebel commandos and CorSec officers** collide as different
  factions try to seize control of the ships." Enemies named: Super Battle Droids, Nova-class
  Stormtroopers. (This is the *ground/interior* Corvette content; the *space* Corvette is the
  pilot-master mission — see §2.4.)

### 1.3 Ongoing space changes documented in update notes — [OFFICIAL]
Sources: `20070126114830/content_001.html` (Publish Archive 2005), `.../content.html` (2006).

- **Deep-space economy:** "Star Destroyer now drops 10–20k credits in deep space"; "Rebel space station
  now drops 10–20k credits in deep space" (2005). Star Destroyer kills grant XP "equivalent to Rebel
  Spacestation in Deep Space."
- **New mission types added over time (2005):** "New destroy and escort missions for Imperial pilots";
  "New … for Privateer pilots"; "New **story-driven** missions for Imperial pilots"; "… for Privateer
  pilots"; Rori-based rebel story missions. → Core mission archetypes are **destroy, escort, story,
  patrol, mining** (mining corroborated in §2.3).
- **Capacitor / weapon overcharge mechanic (official):** "Weapon capacitor overcharge should charge the
  space ship weapons beyond 100% and increase charge rate." A separate note fixes the ship-component
  view ('v'): "Capacitors were not reflecting [correct values]."
- **Multi-crew fixes:** turret gunner's target now shown to pilot; "Credit for a space PvE or PvP kill
  will now be granted to **the entire crew** of a multi-player craft and not just the pilot" (2006).
- **PvP/Kessel:** "Dying in space will cause wounds again" (2005); "If you die during PvP combat in
  space, you will respawn properly" (2006); in Deep Space "when the battle is over … a new winner is
  declared, **Kessel** will clear most of the ships."
- **GCW point tuning (2006):** "Decreased space PvE GCW point per tier value **to 4 from 6**."

### 1.4 Commands / hyperspace — [COMMUNITY, mirrors official UI]
Source: `20070204224219/index.html` ("Full list of new JTL commands — now with hyperspace, droid and
chatter speech"), and `20070127190236/index.html` ("Full list of JTL commands — updated for RotW").

- Booster commands: `/boosterOn`, `/boosterOff`, `/boosterToggle`.
- Faction-locked reactor command: `/reactor` = **"Pump Reactor (Imperial Only)."**
- Hyperspace: `/hyperspaceMap`, `/aborthyperspace`, and per-destination `/hyperspace space_<system>_<n>`
  routes (e.g. `space_corellia_0`, `space_dantooine_1`).

### 1.5 Known balance issue — the "interceptor death-spiral" — [COMMUNITY mechanic, not named in notes]
**Honest finding:** the corpus does **not** contain an official SOE statement naming an
"interceptor death-spiral." However, the mechanic that produces it is documented precisely in the
community damage model (§3.5): components are selected **at random, one per shot**, and a *disabled*
component is **still in the random pool** — if re-selected, the full damage rolls straight to the
chassis. A low-HP component (e.g. a light Droid Interface) dies in one hit, after which hits on its slot
bypass armor entirely — i.e. losing a subsystem accelerates hull loss. Source for the mechanic:
`20070127185925/index.html` ("Know Your Ship"). Treat the named "death-spiral" as a community label for
this documented behavior, not a quoted SOE acknowledgment.

---

## 2. Pilot profession / certification structure

Primary source: `20070204121749/index.html` — **"Pilot FAQ v3.0"** (authored by the official pilot
correspondent TomoRainer; **[OFFICIAL-EP]**). Corroborated by `20070204161420/index.html`
("FAQ and Guide to Pilot v2.0") and `20070205022811/index.html` ("Maudee's Guide to the (Imperial)
Pilot Profession").

### 2.1 Profession framing — [OFFICIAL-EP]
- Pilot **takes no skillpoints** — "anyone can be a pilot." It is a parallel progression to the ground
  professions.
- Training comes **only from pilot trainers** (not player-taught) and is gated on **XP gained + mission
  progression through your squadron**.
- A player can be in **only one squadron at a time**; you may leave to join another squad/faction if you
  meet that faction's requirements.

### 2.2 Three factions × three squadrons (the 9 squadrons) — [OFFICIAL-EP]
"The pilot profession is composed of three different factions, each of which has three available
squadrons to join":

| Faction | Squadrons | Recruiter / notable trainers (location) |
|---|---|---|
| **Rebel** | **Akron's Havoc Squad**; **Vortex Squadron**; **Crimson Phoenix Squadron** | Recruiter J'pai Brek, Tyrena Corellia (-5072,-2343). Havoc: Capt. Kreezo, Tyrena. Vortex: droid V3-FX, Moenia Naboo. Crimson Phoenix: Cmdr Da'la Socuna, Mos Espa Tatooine. |
| **Imperial** | **Inquisition**; **Storm Squadron**; **Black Epsilon** | Recruiter Cmdr Landau, Theed Naboo (-5516,4403). Inquisition: Lt. Barn Sinkko, Kaadara Naboo. Storm: Lt. Akal Colzet, Bestine Tatooine — *"widely considered the hardest pilot squad in the game."* Black Epsilon: Hakasha Sireen, Imp Outpost Talus. |
| **Freelance / Privateer** | **Smuggler Alliance**; **RSF Squadron**; **CorSec Squadron** | Recruiter Gil Burtin, Bestine Tatooine (-1174,-3647). Smuggler Alliance: Dravis, Mos Eisley (*lightly Rebel-aligned; final mission vs Imperial Corvette*). RSF: Capt. Dinge, Theed (*heavily Imperial-aligned; cannot hurt Imperial ships; final mission vs Rebel Corvette*). CorSec: Sgt. Rhea, Coronet Corellia (*true neutral; final mission player chooses Rebel or Imperial Corvette*). |

(Faction names map to the task's list: Imperial Navy = the Imperial squad tree; Rebel Alliance = the
Rebel squad tree; Freelance/Privateer = the Freelance tree. "Vortex / Black Epsilon" and
"Storm / CorSec" in the task brief correspond to the rows above.)

**Ground↔space faction linkage** (`20070204121749/index.html`, message 2):
- Ground **Civilians can ONLY be Privateer** pilots.
- Ground **Imperials** can be an **Imperial OR Privateer** pilot.
- Ground **Rebels** can be a **Rebel OR Privateer** pilot.

### 2.3 Skill-tree layout & tier progression — [OFFICIAL-EP]
- The pilot tree has parallel skill lines (the standard SWG 4-box-per-tier layout):
  - **Xxxx line** = new **ship** certifications (chassis you may fly).
  - **xXxx line** = new **component** certifications (parts you may equip).
  - **xxxX line** = **flight-computer / astromech (droid)** certification — *"a tree all by itself in the
    Pilot Skills."* (`20070127185925/index.html`.)
- **Training requirements per tier** (Pilot FAQ v3.0):
  - **Tier 1:** XP **and** missions
  - **Tier 2:** XP **and** missions
  - **Tier 3:** **missions only**
  - **Tier 4:** XP **and** missions
  - **Master box (`4444`):** **missions only** (two, sometimes three). "Once you train to 4444, XP does
    not matter."
- Maudee's guide confirms boxes are **trained in strict tier order**: "ALL tier 1 boxes must be filled
  to move on to tier 2," etc., with a noted exception: from tier 3→4, take **chassis size before
  equipment** because high-level (L6–7) components won't fit a small chassis.
- **Post-Master ("ace"/prestige):** after mastery "you now receive **prestige points** instead of
  experience points; these are used to gain access to **Deep Space**. **Each faction grants different
  prestige points.**" → the post-master "ace" progression is **prestige-gated Deep Space access**, and
  prestige is **faction-specific** (does not transfer across factions).

### 2.4 Mastery: the Corvette Master Mission — [OFFICIAL-EP]
- The **final mastery mission for every squadron is to destroy a Corellian Corvette** (capital ship) in
  **Kessel**.
- Corvette spawns: ~**2-hour timer**, on alternating hours; will not spawn while its **two gunboat
  escorts** sit at its exit point. Escort gunboats only need to be **disabled (engines killed)** — they
  self-destruct after a minute or two.
- Named spawns (Scylla coords given): Rebel **Corellian Corvette** exits at -7260 4873 6341; Imperial
  **Star Ravager** exits at -6231 -259 -6059.
- Completion requires: hold the mission, be in Kessel & in range when it dies, and have **damaged a
  subsystem** (target with `[` / `]`).

### 2.5 Faction switching / previous-master bonus — [OFFICIAL-EP]
- Pilot skills **cannot be dropped one box at a time** — you drop **all** pilot skills at once; XP and
  prestige reset to zero on resign.
- **Previous-master bonus** = `XP * (1 + number of previous FACTIONAL masters)`; bonus is **per faction
  mastered**, not per squad — up to **3x+1** if you have mastered at least one squad in each of Rebel,
  Imperial, and Privateer.

### 2.6 RotW (Rage of the Wookiees) quest ships — [OFFICIAL-EP]
All quest-granted (must complete the quest yourself to fly): **Heavy X-Wing, Vaksai,
Imperial Guard TIE Interceptor** (Kashyyyk station → Eyma chain), **Y-8 mining ship** (Flash Harrison,
Kachirho), **Eta-2 Actis Interceptor / "Jedi Starfighter"** (Dathomir → Admiral Krieg; requires Imperial
ground combatant), **ARC-170** (Queen of Naboo; requires Rebel ground combatant), **Belbullab-22**
(Grievous' Starship; Myyydril Caverns, kill ~level-92 robot).

---

## 3. Ship / component data (with concrete numbers + source)

### 3.1 Ship roster, tiers, and maneuverability caps — [OFFICIAL-EP / COMMUNITY]
Source: `20070204121749/index.html`, "Complete Ship Stats and Pictures" / **"Message 58"** (credit
SSSnuggles). Cross-ref standalone thread `20070204005445/index.html` ("Complete Ship Stats and
Pictures").

**Legend:** `(Tier) Name : Resource cost ~Mass [SP: guns (MP:+N turret crew)] N missiles
[Spdx, Acc, Dec, Pitch, Yaw, Roll]`. **Max ship speed = engine_speed × 10 × Spdx** (boosters use the
same function). Spdx is a per-chassis speed-multiplier hard cap.

**Rebel:**
- (T0) Z-95 : 5k ~12k mass [1 gun] 1 msl [Spdx 0.95, Acc 50 Dec 50, P600 Y600 R300]
- (T0) Heavy Z-95 : 50k ~95k [1 gun] 1 msl [0.95, 50/50, 600/600/300]
- (T1) Y-Wing : 15k ~150k [1 gun MP:+1] 1 msl [0.85, 15/25, 80/80/40]
- (T2) Y-Wing LP : 30k ~170k [2 guns] 2 msl [0.85, 10/20, 70/70/35]
- (T3) X-Wing : 60k ~100k [3 guns] 1 msl [0.95 foils-closed / 0.90 open, 25/30, 300/200/150]
- (T4) A-Wing : 80k ~65k [1 gun] 1 msl [1.0, 40/60, 250/250/125]
- (T4) Heavy X-Wing : 180k mass [3 guns] 1 msl [0.95/0.90, 25/30, 300/200/150] — quest-given
- (T5) B-Wing : 150k ~240k [4 guns] 2 msl [0.85/0.80, 10/15, 50/50/25]
- (T5) Nova Courier : 150k ~5m mass [0 guns MP:+2] 3 msl [0.8, 5/5, 50/50/37.5] — POB

**Freelance / Privateer:**
- (T0) Scyk : 5k ~12k [1 gun] 1 msl [0.95, 50/50, 600/600/300]
- (T0) Heavy Scyk : 50k ~95k [1 gun] 1 msl [0.95, 50/50, 600/600/300]
- (T1) Dunelizard : 15k ~85k [2 guns] 1 msl [0.95, 30/30, 300/300/150]
- (T2) Kihraxz : 30k ~40k [1 gun] 1 msl [1.0, 40/50, 400/400/200]
- (T2) Kimogila : 30k ~110k [3 guns] 1 msl [0.92, 35/35, 100/100/50]
- (T3) Ixiyen : 65k ~95k [2 guns] 1 msl [0.95, 25/30, 200/200/100]
- (T4) Rihxyrk : 90k ~190k [3 guns] 1 msl [0.89, 35/35, 100/100/50]
- (T4) Vaksai : 150k mass [1 gun] 1 msl [1.0, 40/50, 400/400/200] — quest-given
- (T5) YT-1300 : 150k ~5m [0 guns MP:+2] 2 msl [0.8, 5/5, 50/50/37.5] — POB
- (T5) M22-T Krayt : 150k ~210k [3 guns MP:+1] 1 msl [0.92, ~50/~50, 100/100/50]

**Imperial:**
- (T0) TIE-LD : 5k ~12k [1 gun] **0 missiles** [0.95, 50/50, 600/600/300]
- (T1) TIE-F : 15k ~20k [1 gun] 1 msl [0.98, 40/50, 500/500/250]
- (T1) Heavy TIE Fighter : 50k ~95k [1 gun] 1 msl [0.98, 50/50, 500/500/250]
- (T2) TIE/IN : 30k ~40k [1 gun] 1 msl [0.97, 40/60, 300/300/200]
- (T3) TIE-Bomber : 60k ~190k [2 guns] 2 msl [0.85, 10/15, 50/50/25]
- (T3) TIE-Interceptor : 60k ~50k [1 gun] 1 msl [1.0, 40/60, 300/300/150]
- (T4) TIE-Advanced : 80k ~65k [2 guns] 1 msl [1.0, 40/60, 300/300/150]
- (T4) TIE-Aggressor : 80k ~120k [1 gun MP:+1] 1 msl [0.95, 20/30, 150/150/75]
- (T4) Imperial Guard TIE Interceptor : 80k mass [1 gun] 1 msl [1.0, 40/60, 300/300/150] — quest-given
- (T5) TIE-Oppressor : 150k ~170k [2 guns] 3 msl [0.9, 35/35, 100/100/50]
- (T5) TIE-Decimator : 150k ~5m [0 guns MP:+2] 3 msl [Spdx ??, 5/5, 50/50/37.5] — POB

**Cross-faction / special:**
- KSE Firespray (T5) : 150k ~210k [2 guns] 2 msl [0.9, ?/?, 100/100/50] — *"Every Master Pilot gains
  certification in it."* Schematic not from master shipwright (separate quest).
- ARC-170 (T4) : 125k mass [1 gun MP:+2] 1 msl [0.97, ?/?, 400/400/200] — quest-given, multi-player
- Eta-2 Actis Interceptor (T4) : 90k mass [2 guns] 1 msl [0.95, 50/50, 600/600/300] — quest-given
- Belbullab-22 (T4) : 90k mass [2 guns] 1 msl [0.95, 50/50, 600/600/300] — quest-given
- Sorosuub 3000 "Lady Luck" : 180-day veteran reward (also pre-order yacht per §1.1)

**POB / multi-player ships** (`20070204121749/index.html`, message 2): POB ("portalized object",
walk-around) = **Yacht, YT-1300, Decimator, Nova Courier, Y-8**. Multi-player (turret gunners) =
**Krayt, Aggressor, Y-Wing, ARC-170** plus all POB ships.

> Confidence: these stat values are **player-measured [COMMUNITY]**, but they are the canonical Pre-CU
> pilot-correspondent reference and align with SWGEmu's reverse-engineered chassis values. Spdx and
> P/Y/R are the chassis hard caps; `Acc`/`Dec` are acceleration/deceleration; mass is the chassis
> baseline before components.

### 3.2 Component types & the "mass budget" model — [OFFICIAL-EP]
Source: `20070204123359/index.html` — **"Mass Management: A Community Guide (OFFICIAL)"**, and
`20070127185925/index.html` ("Know Your Ship").

The component slots a ship fits are: **Reactor, Engine, Shield, Armor (front/back), Weapon(s),
Capacitor, Booster, Droid Interface (DI) + flight-computer/astromech.** Core mechanic: every component
has a **mass**, every chassis has a **mass cap**, and the **Reactor supplies power** that every powered
component draws — so building a ship is a mass-vs-power-vs-performance budget. "Higher-level ships and
components aren't necessarily better… if you're close to filling up your ship's mass capacity, it may be
wise to use some lower-level (and thus lower mass) components."

### 3.3 Component certification levels (the "even-level" rule) — [OFFICIAL-EP]
Source: `20070204123359/index.html` (Mass Management).

- Components come in **certification levels 1–10**.
- **Capacitors & Boosters:** *"Even levels are where it's at … 2, 4, 6, 8, 10. Pretty much ignore 1,3,5,
  7,9"* — i.e. meaningful stat breakpoints occur on even certification levels for these two parts (best
  RE results come from even-level loot drops; "Crafted Capacitors are completely useless," per Know Your
  Ship).
- Reward/quest components are noted as top-tier: e.g. "Every L6 reward [engine] is in the top 1% for
  speed for L6 engines." Looted L7 reactors observed at ~30.8k–39k power (RE to ~41k).

### 3.4 Per-component stats that matter — [OFFICIAL-EP] (`20070127185925/index.html`, "Know Your Ship")
- **Shield:** Front HP + Back HP + **recharge rate (units/sec, PER SIDE)**. Properly-crafted or RE'd
  shields beat straight-looted. Watch for naturally **unbalanced** shields (e.g. 30% front-heavy + 30%
  back-heavy RE'd together → more total HP than an even shield). **Do not load loot shields directly into
  a ship** (auto-loading triggers the manufactured-equality normalization).
- **Capacitor:** **Recharge Rate (units/sec)** + **total Energy stored**. Weapons drain the cap per
  shot; if drain rate > recharge you run dry. Example given: a single weapon at 0.33 refire taking 75
  units/sec against a 25 recharge depletes twice as fast as it refills.
- **Reactor:** supplies all power; crafted reactors generally beat looted/RE. A L1 crafted reactor can
  exceed 10k power at very low mass (used as a light "bootup" unit).
- **Engine:** governs speed (× Spdx, §3.1). **All acceleration is dictated by the chassis**, not the
  engine.
- **Booster:** Booster Energy + Energy Consumption Rate + Recharge Rate; trades mass/reactor draw for
  burst speed/acceleration. RE'd boosters always have higher mass than crafted.
- **Weapon:** **Damage**, **Energy/shot** (lower better), **refire rate**. RE'd weapons are the only ones
  with "serious offensive power" at endgame. Weapon types: **Blaster, Ion, Disruptor**; missiles include
  anti-shield, armor-piercing, **Seismic** and **Concussion**, and **Mark II space bombs** (tear apart
  capital/high-tier ships).
- **Droid Interface / flight computer:** runs droid programs (e.g. Shield Reinforcement, overloads); the
  DI's **speed** stat governs how often a program can fire (a 30-speed DI → a program can re-trigger
  every 30 s). Astromechs (X-/Y-/Z-95/Eta-2 only) **count against the datapad droid limit**, are
  tradeable/nameable, and are crafted by Droid Engineers; flight computers do **not** count and are
  **not** tradeable.

### 3.5 Damage-resolution model — [COMMUNITY] (`20070127185925/index.html`, "Know Your Ship")
The order a shot resolves (the mechanic behind the §1.5 death-spiral):
1. Hit **shield** on the facing side → apply **vs-shield** multiplier → subtract from shot damage.
2. Remaining damage hits **armor** → apply **vs-armor** multiplier → subtract.
3. **One component is selected at random** (one per shot). Damage applies to that component's armor then
   HP; if HP hits 0 the **remainder spills to the chassis**.
4. A **disabled component stays in the random pool** — if re-selected, full remaining damage goes
   straight to chassis. Low-HP parts (e.g. a weak DI) die in one hit and statistically appear to be "hit
   more," but all components are equally likely to be selected. **This is why losing subsystems
   accelerates hull death.**

### 3.6 Reverse engineering — [COMMUNITY]
Sources: `20070202014228/index.html` ("Reverse Engineering Levels of Equipment"),
`20070202011810/index.html` ("Guide To Reverse Engineering"), `20070127201527/index.html` ("Zigmund's
Shipwright Guide"). RE takes looted components and re-rolls/improves stats; it is the dominant endgame
source for shields, capacitors, boosters, and weapons. (The RE-Levels thread is largely a weapon
**graphic/color** cross-reference per certification level rather than a numeric stat table; concrete RE
numeric caps are **not fully documented** in this corpus.)

---

## 4. Squadron / faction content & themepark structure

- **9 squadrons across 3 factions** — full table in §2.2. Each squadron is a self-contained
  themepark: a recruiter + a chain of squadron-specific missions culminating in the **Corvette Master
  Mission** (§2.4). [OFFICIAL-EP, `20070204121749/index.html`]
- **Mission archetypes:** destroy, escort, patrol, story-driven, and **space mining** (Y-8/Red-Dwarf's
  mining guide referenced). New destroy/escort/story missions were added per faction in 2005 update
  notes. [OFFICIAL §1.3]
- **Deep Space / Kessel** is the prestige-gated endgame zone: master pilots spend faction prestige to
  reach Deep Space via factional "Deep Space – Kessel jump" stations (Dantooine space for Rebel, Endor
  space for Imperial). Kessel hosts the Corvette spawns and PvP, and clears ships when a battle is
  decided. [OFFICIAL-EP §2.4 + OFFICIAL §1.3]
- **Black Sun** content: tier-4+ Black Sun ships rarely drop a **Black Sun Ace Helmet** (extremely rare
  loot). [OFFICIAL-EP §RotW]
- **Multi-crew operations** (`20070127190007/index.html` "Multi-Player Ship Operation Guide"): POB ships
  use a pilot + operations officer + turret gunners; reactor/capacitor must sustain weapon/engine
  overloads; kill credit is shared across the whole crew (§1.3).

---

## 5. Live-faithfulness checklist (map Live facts → what the port must match)

Use this to audit the Stardust space port against Live. Each row cites the §/source above.

| # | Live fact (source) | Port should match | Status to verify |
|---|---|---|---|
| 1 | Pilot is **skillpoint-free**, trainer-only, gated on **XP + squadron missions** (§2.1) | Pilot progression costs 0 skill points; advancement requires mission completion, not just XP | ☐ |
| 2 | **3 factions × 3 squadrons = 9** squadrons with the exact names/recruiters in §2.2 | All 9 squadrons present with correct faction alignment & recruiter NPCs/coords | ☐ |
| 3 | Tier gating: T1 xp+msn, T2 xp+msn, **T3 msn-only**, T4 xp+msn, **Master msn-only** (§2.3) | Per-tier unlock requirements match exactly (esp. T3 and Master = missions only) | ☐ |
| 4 | Skill lines split: **ships (Xxxx) / components (xXxx) / droid (xxxX)** (§2.3) | Cert trees separate ship vs component vs flight-droid certs | ☐ |
| 5 | Post-master uses **faction-specific prestige**, not XP; gates **Deep Space** (§2.3) | Prestige currency per faction; Deep Space/Kessel access gated by it; prestige resets on faction change | ☐ |
| 6 | **Previous-master bonus = XP × (1 + #factional masters)**, max 3x+1 (§2.5) | Master-bonus formula matches; counted per faction not per squad | ☐ |
| 7 | Mastery = **destroy a Corellian Corvette in Kessel** (§2.4) | Each squad's final mission is the Corvette kill; subsystem-damage + in-range completion rules | ☐ |
| 8 | Ground↔space faction linkage (Civilian→Privateer only, etc.) (§2.2) | Pilot-faction eligibility tied to ground faction per the rule | ☐ |
| 9 | **Max speed = engine_speed × 10 × Spdx**; Spdx is per-chassis cap (§3.1) | Speed formula and per-chassis Spdx caps match the table | ☐ |
| 10 | Full chassis roster: tier, mass, gun/turret count, missile count, Spdx/Acc/Dec/P/Y/R (§3.1) | Each chassis's tier, mass, hardpoints, and maneuverability caps match the §3.1 table | ☐ |
| 11 | Component model: Reactor powers all; every part has **mass**; chassis has **mass cap** (§3.2) | Reactor-power + mass-budget enforced; over-mass ships can't load | ☐ |
| 12 | Components are **cert level 1–10**; capacitors/boosters meaningful on **even levels** (§3.3) | Component cert levels 1–10; even-level breakpoints for cap/booster | ☐ |
| 13 | Shield = front/back HP + **per-side recharge (u/s)**; cap = energy + recharge; weapon drains cap/shot (§3.4) | Shield/capacitor/weapon energy economy modeled per-side & per-shot | ☐ |
| 14 | **All acceleration is chassis-dictated**, not engine (§3.4) | Engine sets speed only; accel/decel from chassis | ☐ |
| 15 | Damage order: shield(vs-shield) → armor(vs-armor) → **1 random component/shot**, disabled parts still in pool → spill to chassis (§3.5) | Damage resolution + random-component selection + spillover match (this *is* the "death-spiral") | ☐ |
| 16 | Astromechs (X/Y/Z-95/Eta-2 only) count vs droid limit & are crafted by DE; flight computers don't count / not tradeable (§3.4) | Droid/flight-computer rules and ship eligibility match | ☐ |
| 17 | RE is the endgame stat source for shields/caps/boosters/weapons (§3.6) | Reverse-engineering present and dominant for endgame components | ☐ |
| 18 | Mission archetypes: destroy, escort, patrol, story, **mining**; Deep-space loot 10–20k cr from capital ships (§1.3) | Mission types + deep-space credit drops match | ☐ |
| 19 | Multi-crew: POB list (Yacht/YT-1300/Decimator/Nova Courier/Y-8); MP turret list; **kill credit shared across crew** (§3.1, §1.3) | POB & multi-crew ship sets correct; shared kill credit | ☐ |
| 20 | RotW ships are all quest-granted with the specific quest chains (§2.6) | RotW ships gated behind their quests, not free certs | ☐ |

---

## Appendix A — Highest-value source files (paths relative to `…/data/`)

| File | Title / content |
|---|---|
| `20070204121749/index.html` | **Pilot FAQ v3.0** — cert tiers, 9 squadrons, prestige/Deep Space, full ship stat table ("Message 58"), POB/MP lists |
| `20070204161420/index.html` | FAQ and Guide to Pilot v2.0 (corroborates squadrons/ships) |
| `20070205022811/index.html` | Maudee's Guide to the (Imperial) Pilot Profession — tier-order training, component priorities |
| `20070204005445/index.html` | Complete Ship Stats and Pictures (standalone) |
| `20070204123359/index.html` | **Mass Management: A Community Guide (OFFICIAL)** — mass budget, even-level cert rule |
| `20070127185925/index.html` | **Know Your Ship** — per-component stats + damage-resolution model |
| `20070127190007/index.html` | Multi-Player Ship Operation Guide v1.30 — POB/gunner/overload mechanics |
| `20070127201527/index.html` | Zigmund's Shipwright Guide |
| `20070202011810/index.html` | Guide To Reverse Engineering |
| `20070202014228/index.html` | Reverse Engineering Levels of Equipment (weapon graphic/level cross-ref) |
| `20070205032803/index.html` | Weapons Data — Master List (Core Systems) |
| `20070204224219/index.html` | Full list of new JTL commands (hyperspace/droid/chatter) |
| `20070127190236/index.html` | Full list of JTL commands — updated for RotW |
| `20070126114830/content_002.html` | **Publish Archive 2004** [OFFICIAL] — JTL launch (Pub 11), Corvette themepark |
| `20070126114830/content_001.html` | **Publish Archive 2005** [OFFICIAL] — space missions, Deep Space, capacitor overcharge |
| `20070126114830/content.html` | **Publish Archive 2006** [OFFICIAL] — multi-crew kill credit, Kessel, GCW point tuning |

## Appendix B — What is NOT in the corpus (gaps; do not invent)
- No official SOE statement explicitly naming an "interceptor death-spiral" (the *mechanic* is documented
  — §1.5/§3.5 — but the label is community usage).
- No first-party raw numeric stat tables for components (SOE never published these; all component numbers
  here are player-measured [COMMUNITY] and should be cross-checked against SWGEmu RE'd values).
- Complete numeric RE stat caps per component level are not tabulated in this corpus.
- A few chassis values are marked `?`/`??` in the source (e.g. TIE-Decimator Spdx, ARC-170 Acc/Dec) and
  remain unknown from this corpus.
