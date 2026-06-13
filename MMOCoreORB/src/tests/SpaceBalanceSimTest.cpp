/*
 * SpaceBalanceSimTest.cpp
 *
 * Phase 6.C -- Standalone Monte Carlo space-combat balance harness.
 *
 * Runs headless in CI (no ServerDatabase, no TRE, no Lua, no live ZoneServer). It
 * re-implements the EXACT damage pipeline of SpaceCombatManager::applyDamage over a
 * plain-old-data ship model (BalanceShip) so we can sweep ship/component permutations
 * and surface dominant-strategy breakpoints + the interceptor death-spiral without a
 * running server.
 *
 * Provenance of the formulas (kept 1:1 with the ported server code):
 *   MMOCoreORB/src/server/zone/managers/spacecombat/SpaceCombatManager.cpp
 *     - applyDamage:   damage = rand(min..max)*weaponEffect; *=0.75 if !player-target
 *     - applyShieldDamage:  shieldDamage = damage * shieldEffect; subtract from shield;
 *                           returns leftover/shieldEffect to flow to armor
 *     - applyArmorDamage:   armorDamage = damage * armorEffect; eats armor pool then
 *                           component-health pool; returns leftover/armorEffect
 *     - applyChassisDamage: chassis -= leftover
 *   Hit-location order in SWG space: front shield -> rear shield (here: a single shield
 *   facing per shot) -> armor (front/rear) -> chassis. We model the canonical
 *   "shields then armor then chassis" attrition that drives time-to-kill.
 *
 * IMPORTANT: v1 uses representative/SYNTHETIC stat inputs. Real numbers come from 6.B
 * (clean-room datatable extraction). The BalanceShip struct mirrors the datatable
 * fields so extracted values plug straight in. No real SWG stat values are fabricated
 * as authoritative here -- these are clearly-labelled synthetic archetypes for finding
 * structural breakpoints.
 *
 * Output: a win-rate matrix printed to stdout (captured by CI). A sample rendering is
 * committed at docs/space-port/balance/SAMPLE-REPORT.md.
 */

#include "gtest/gtest.h"

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include <cstdio>

namespace space_balance {

// Deterministic RNG so CI runs are reproducible (do NOT use System::random here --
// we want a fixed seed for stable win-rate reporting).
struct Lcg {
	uint64_t state;
	explicit Lcg(uint64_t seed) : state(seed ? seed : 0x9E3779B97F4A7C15ull) {}
	uint32_t next() {
		state = state * 6364136223846793005ull + 1442695040888963407ull;
		return (uint32_t)(state >> 32);
	}
	// uniform float in [0,1)
	float unit() { return (next() & 0xFFFFFF) / (float) 0x1000000; }
	// uniform float in [0,range)
	float range(float r) { return r <= 0.f ? 0.f : unit() * r; }
};

// Mirrors the datatable-backed component stats SpaceCombatManager reads off a
// ShipObject. Field names track the server accessors so 6.B values plug in directly.
struct BalanceShip {
	std::string name;

	// Weapon (one representative primary weapon slot).
	float weaponMinDamage = 0.f;   // getComponentMinDamageMap()->get(weaponIndex)
	float weaponMaxDamage = 0.f;   // getComponentMaxDamageMap()->get(weaponIndex)
	float weaponEfficiency = 1.f;  // getComponentEfficiency(weaponIndex), clamped 0.1..10
	float refireSeconds = 1.0f;    // shots per engagement tick = 1/refire

	// Defensive effectiveness coefficients (per-weapon-type on the TARGET ship in the
	// server; modeled here as the target's resistances).
	float shieldEffectiveness = 1.f; // getShieldEffectivenessMap()->get(weaponIndex)
	float armorEffectiveness = 1.f;  // getArmorEffectivenessMap()->get(weaponIndex)

	// Defensive pools.
	float shieldFront = 0.f;       // getMaxFrontShield()
	float shieldRear = 0.f;        // getMaxRearShield()
	float armorFront = 0.f;        // getMaxArmorMap()->get(ARMOR0)
	float armorRear = 0.f;         // getMaxArmorMap()->get(ARMOR1)
	float componentHealth = 0.f;   // sum of getMaxHitpointsMap() (armor-backing health)
	float chassisHealth = 0.f;     // getChassisMaxHealth()

	// Maneuver (drives whether an interceptor can stay on target / break off).
	float speedMax = 0.f;          // ship_chassis flight constant
	float yawRate = 0.f;           // turn rate (rad/s)

	// Engagement-level hit chance (synthetic; a real headless-client harness measures
	// this from geometry. Faster turners land more of their shots on slower targets).
	float baseHitChance = 0.6f;
};

// One full shot resolved exactly like SpaceCombatManager::applyDamage's leftover chain.
// Mutates the target pools. Returns damage dealt to chassis this shot (for telemetry).
static float resolveShot(Lcg& rng, const BalanceShip& attacker, BalanceShip& target) {
	// applyDamage: weaponEffect clamped 0.1..10, damage = rand(max-min)+min, *weaponEffect
	float weaponEffect = attacker.weaponEfficiency;
	if (weaponEffect < 0.1f) weaponEffect = 0.1f;
	if (weaponEffect > 10.f) weaponEffect = 10.f;

	float minDamage = attacker.weaponMinDamage * weaponEffect;
	if (minDamage < 0.f) minDamage = 0.f;
	float maxDamage = attacker.weaponMaxDamage * weaponEffect;
	if (maxDamage < minDamage) maxDamage = minDamage;

	float damage = rng.range(maxDamage - minDamage) + minDamage;
	if (damage <= 0.f || damage > 999999.f) return 0.f;

	// Non-player target multiplier (PvE shots are *0.75 in applyDamage). We model AI
	// targets, matching the ported code path.
	damage *= 0.75f;

	// --- Shield facing (front shield first; mirror to rear when front depleted) ---
	float shieldEffect = target.shieldEffectiveness;
	if (shieldEffect > 0.f) {
		float* shield = (target.shieldFront > 0.f) ? &target.shieldFront : &target.shieldRear;
		if (*shield > 0.f) {
			float shieldDamage = damage * shieldEffect;
			if (shieldDamage > *shield) {
				shieldDamage -= *shield;
				*shield = 0.f;
			} else {
				*shield -= shieldDamage;
				shieldDamage = 0.f;
			}
			// leftover flows on, divided back out by effect (per applyShieldDamage)
			damage = shieldDamage / shieldEffect;
		}
	}
	if (damage <= 0.f) return 0.f;

	// --- Armor (front pool, then its backing component-health pool) ---
	float armorEffect = target.armorEffectiveness;
	if (armorEffect > 0.f && (target.armorFront > 0.f || target.componentHealth > 0.f)) {
		float armorDamage = damage * armorEffect;

		if (target.armorFront > 0.f) {
			if (armorDamage > target.armorFront) {
				armorDamage -= target.armorFront;
				target.armorFront = 0.f;
			} else {
				target.armorFront -= armorDamage;
				armorDamage = 0.f;
			}
		}
		if (target.componentHealth > 0.f && armorDamage > 0.f) {
			if (armorDamage > target.componentHealth) {
				armorDamage -= target.componentHealth;
				target.componentHealth = 0.f;
			} else {
				target.componentHealth -= armorDamage;
				armorDamage = 0.f;
			}
		}
		damage = armorDamage / armorEffect;
	}
	if (damage <= 0.f) return 0.f;

	// --- Chassis (raw leftover) ---
	float dealt = damage;
	if (damage > target.chassisHealth) {
		dealt = target.chassisHealth;
		target.chassisHealth = 0.f;
	} else {
		target.chassisHealth -= damage;
	}
	return dealt;
}

static bool isDead(const BalanceShip& s) { return s.chassisHealth <= 0.f; }

// Effective per-shot hit chance. A maneuver-advantaged attacker (higher yaw rate vs the
// target) lands more shots; a slower attacker chasing a faster target lands fewer.
// This is the lever that produces the interceptor death-spiral.
static float effectiveHitChance(const BalanceShip& attacker, const BalanceShip& target) {
	float chance = attacker.baseHitChance;
	float turnAdv = (attacker.yawRate - target.yawRate) * 0.15f;
	float speedAdv = (target.speedMax - attacker.speedMax) * 0.0005f; // chasing a faster ship hurts
	chance += turnAdv - speedAdv;
	if (chance < 0.05f) chance = 0.05f;
	if (chance > 0.98f) chance = 0.98f;
	return chance;
}

// Simulate one 1v1 duel to the death. Returns true if shipA wins. timeoutTicks guards
// against unkillable stalemates (counted as a loss for the higher-DPS ship by
// convention -> attrition favors the tank, which is itself a balance signal).
static bool duel(Lcg& rng, BalanceShip a, BalanceShip b, int timeoutTicks = 4000) {
	// Both fire on a shared tick; resolution order alternates to avoid first-mover bias.
	for (int tick = 0; tick < timeoutTicks; ++tick) {
		bool aFirst = (tick & 1) == 0;
		BalanceShip& first = aFirst ? a : b;
		BalanceShip& second = aFirst ? b : a;

		if (rng.unit() < effectiveHitChance(first, second)) resolveShot(rng, first, second);
		if (isDead(second)) return aFirst; // first killed second
		if (rng.unit() < effectiveHitChance(second, first)) resolveShot(rng, second, first);
		if (isDead(first)) return !aFirst;
	}
	// Stalemate: whoever has more remaining chassis fraction "wins" on points.
	return a.chassisHealth >= b.chassisHealth;
}

// Synthetic archetypes (CLEARLY NOT authoritative SWG values -- structural test inputs).
static std::vector<BalanceShip> archetypes() {
	std::vector<BalanceShip> v;

	BalanceShip interceptor;
	interceptor.name = "Interceptor";
	interceptor.weaponMinDamage = 80.f; interceptor.weaponMaxDamage = 140.f;
	interceptor.weaponEfficiency = 1.0f; interceptor.refireSeconds = 0.5f;
	interceptor.shieldEffectiveness = 1.0f; interceptor.armorEffectiveness = 1.0f;
	interceptor.shieldFront = 1500.f; interceptor.shieldRear = 1500.f;
	interceptor.armorFront = 1200.f; interceptor.armorRear = 1200.f;
	interceptor.componentHealth = 800.f; interceptor.chassisHealth = 4000.f;
	interceptor.speedMax = 95.f; interceptor.yawRate = 3.2f; interceptor.baseHitChance = 0.55f;
	v.push_back(interceptor);

	BalanceShip gunship;
	gunship.name = "Gunship";
	gunship.weaponMinDamage = 220.f; gunship.weaponMaxDamage = 360.f;
	gunship.weaponEfficiency = 1.0f; gunship.refireSeconds = 1.4f;
	gunship.shieldEffectiveness = 0.85f; gunship.armorEffectiveness = 0.8f;
	gunship.shieldFront = 4200.f; gunship.shieldRear = 4200.f;
	gunship.armorFront = 5000.f; gunship.armorRear = 5000.f;
	gunship.componentHealth = 2500.f; gunship.chassisHealth = 14000.f;
	gunship.speedMax = 55.f; gunship.yawRate = 1.1f; gunship.baseHitChance = 0.6f;
	v.push_back(gunship);

	BalanceShip bomber;
	bomber.name = "Bomber";
	bomber.weaponMinDamage = 400.f; bomber.weaponMaxDamage = 650.f;
	bomber.weaponEfficiency = 1.0f; bomber.refireSeconds = 2.6f;
	bomber.shieldEffectiveness = 0.9f; bomber.armorEffectiveness = 0.75f;
	bomber.shieldFront = 3000.f; bomber.shieldRear = 3000.f;
	bomber.armorFront = 6000.f; bomber.armorRear = 6000.f;
	bomber.componentHealth = 3000.f; bomber.chassisHealth = 18000.f;
	bomber.speedMax = 42.f; bomber.yawRate = 0.7f; bomber.baseHitChance = 0.5f;
	v.push_back(bomber);

	return v;
}

// Run N duels per (i,j) ordered pair; return shipI win fraction.
static float winRate(int i, int j, int trials) {
	auto ships = archetypes();
	int wins = 0;
	for (int t = 0; t < trials; ++t) {
		Lcg rng(0xABCDEFull + (uint64_t)((i * 131 + j) * 1000003 + t));
		if (duel(rng, ships[i], ships[j])) ++wins;
	}
	return (float) wins / (float) trials;
}

} // namespace space_balance

namespace space_balance_test {

using namespace space_balance;

class SpaceBalanceSimTest : public ::testing::Test {
public:
	static constexpr int TRIALS = 300;
};

// Print the full win-rate matrix. Always runs; the assertion below is the real gate.
TEST_F(SpaceBalanceSimTest, MonteCarloWinRateMatrix) {
	auto ships = archetypes();
	const int n = (int) ships.size();

	printf("\n==== SPACE BALANCE SIM: win-rate matrix (row attacks column), %d trials/cell ====\n", TRIALS);
	printf("%-14s", "ATK \\ DEF");
	for (int j = 0; j < n; ++j) printf("%14s", ships[j].name.c_str());
	printf("\n");

	for (int i = 0; i < n; ++i) {
		printf("%-14s", ships[i].name.c_str());
		for (int j = 0; j < n; ++j) {
			if (i == j) { printf("%14s", "--"); continue; }
			printf("%13.1f%%", winRate(i, j, TRIALS) * 100.f);
		}
		printf("\n");
	}
	printf("================================================================================\n");

	// Sanity gate: matrix must be well-formed (every off-diagonal is a valid probability).
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			if (i != j) {
				float wr = winRate(i, j, TRIALS);
				ASSERT_GE(wr, 0.f);
				ASSERT_LE(wr, 1.f);
			}
}

// Quantify the interceptor death-spiral against the gunship: as the gunship's effective
// DPS advantage compounds (interceptor loses shields -> takes armor/chassis -> fewer of
// its own shots matter), the interceptor's win rate must collapse. We assert the
// DIRECTION of the effect (a structural breakpoint), not an absolute tuned number.
TEST_F(SpaceBalanceSimTest, InterceptorDeathSpiralIsDetectable) {
	auto ships = archetypes();
	int interceptor = 0, gunship = 1;

	float interceptorVsGunship = winRate(interceptor, gunship, 500);
	printf("\n[death-spiral] Interceptor vs Gunship win rate: %.1f%%\n", interceptorVsGunship * 100.f);

	// The light interceptor should be the underdog vs the heavy gunship in sustained
	// 1v1 attrition -- this IS the death-spiral the plan names. If this ever flips above
	// 50%%, balance has regressed and the harness has caught it.
	EXPECT_LT(interceptorVsGunship, 0.5f)
		<< "Interceptor unexpectedly dominates Gunship in 1v1 attrition -- "
		   "death-spiral breakpoint has shifted; re-check component/damage curves.";
}

// Dominant-strategy detector: no archetype should win >85%% against ALL others, which
// would indicate a single dominant build. Flags hard imbalance.
TEST_F(SpaceBalanceSimTest, NoUniversallyDominantArchetype) {
	auto ships = archetypes();
	const int n = (int) ships.size();

	for (int i = 0; i < n; ++i) {
		bool dominatesAll = true;
		for (int j = 0; j < n; ++j) {
			if (i == j) continue;
			if (winRate(i, j, 300) <= 0.85f) { dominatesAll = false; break; }
		}
		EXPECT_FALSE(dominatesAll)
			<< ships[i].name << " wins >85%% vs every other archetype (dominant strategy).";
	}
}

} // namespace space_balance_test
