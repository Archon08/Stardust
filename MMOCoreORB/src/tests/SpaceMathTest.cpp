/*
 * SpaceMathTest.cpp
 *
 * Phase 6 (verification) headless unit tests for the ported space math.
 *
 * These tests exercise ONLY the pure-computation overloads of SpaceMath and the
 * SpaceTransform value type. They require no ServerDatabase, no TRE files, no Lua
 * runtime and no live ZoneServer -- they run entirely headless in CI. They are the
 * highest-confidence cloud-runnable verification of the space-port transplant.
 *
 * Deliberately NOT covered here (require server/TRE/Lua state, see space-tests.yml
 * documentation and SCENARIO-MATRIX.md):
 *   - SpaceMath::getIntersection(ShipObject*, ...) / (AppearanceTemplate*, ...)
 *       -> needs a ShipObject with a loaded appearance mesh from TRE.
 *   - SpaceMath::rotationToQuaternion(rotation, precision=false)
 *       -> needs RotationLookupTable singleton populated.
 *   - ShipObjectTransform interpolation
 *       -> all update paths take a live ShipObject*.
 */

#include "gtest/gtest.h"

#include "server/zone/objects/ship/transform/SpaceMath.h"
#include "server/zone/objects/ship/transform/SpaceTransform.h"

namespace server {
namespace zone {
namespace objects {
namespace ship {
namespace transform {
namespace test {

class SpaceMathTest : public ::testing::Test {
public:
	// Tolerance for float comparisons. SpaceMath uses single precision sqrtf/cosf/sinf,
	// and qInvSqrt is exact (1/sqrtf) so a tight epsilon is fine for these scales.
	static constexpr float EPS = 1e-4f;

	void SetUp() override {}
	void TearDown() override {}
};

// ----- qInvSqrt / qSqrt: defensive zero/negative handling -----

TEST_F(SpaceMathTest, QSqrtReturnsZeroForNonPositive) {
	EXPECT_FLOAT_EQ(SpaceMath::qSqrt(0.f), 0.f);
	EXPECT_FLOAT_EQ(SpaceMath::qSqrt(-4.f), 0.f);
}

TEST_F(SpaceMathTest, QSqrtMatchesSqrtfForPositive) {
	EXPECT_NEAR(SpaceMath::qSqrt(16.f), 4.f, EPS);
	EXPECT_NEAR(SpaceMath::qSqrt(2.f), sqrtf(2.f), EPS);
}

TEST_F(SpaceMathTest, QInvSqrtReturnsZeroForNonPositive) {
	EXPECT_FLOAT_EQ(SpaceMath::qInvSqrt(0.f), 0.f);
	EXPECT_FLOAT_EQ(SpaceMath::qInvSqrt(-1.f), 0.f);
}

TEST_F(SpaceMathTest, QInvSqrtMatchesReciprocalSqrt) {
	EXPECT_NEAR(SpaceMath::qInvSqrt(16.f), 0.25f, EPS);
	EXPECT_NEAR(SpaceMath::qInvSqrt(4.f), 0.5f, EPS);
}

// ----- qNormalize(Vector3): returns original length, normalizes in place -----

TEST_F(SpaceMathTest, QNormalizeVectorReturnsLengthAndUnitizes) {
	Vector3 v(3.f, 4.f, 0.f); // length 5
	float len = SpaceMath::qNormalize(v);

	EXPECT_NEAR(len, 5.f, EPS);
	EXPECT_NEAR(v.getX(), 0.6f, EPS);
	EXPECT_NEAR(v.getY(), 0.8f, EPS);
	EXPECT_NEAR(v.getZ(), 0.f, EPS);

	// Result must be unit length.
	EXPECT_NEAR(SpaceMath::qSqrt(v.squaredLength()), 1.f, EPS);
}

TEST_F(SpaceMathTest, QNormalizeZeroVectorIsSafe) {
	Vector3 v(0.f, 0.f, 0.f);
	float len = SpaceMath::qNormalize(v);

	EXPECT_FLOAT_EQ(len, 0.f);
	// Must not divide by zero / produce NaN.
	EXPECT_FALSE(std::isnan(v.getX()));
	EXPECT_FALSE(std::isnan(v.getY()));
	EXPECT_FALSE(std::isnan(v.getZ()));
}

// ----- qNormalize(Quaternion): unit-quaternion normalize -----

TEST_F(SpaceMathTest, QNormalizeQuaternionUnitizes) {
	Quaternion q(2.f, 0.f, 0.f, 0.f); // length 2
	float len = SpaceMath::qNormalize(q);

	EXPECT_NEAR(len, 2.f, EPS);
	EXPECT_NEAR(SpaceMath::qSqrt(q.lengthSquared()), 1.f, EPS);
}

TEST_F(SpaceMathTest, QNormalizeZeroQuaternionIsSafe) {
	Quaternion q(0.f, 0.f, 0.f, 0.f);
	float len = SpaceMath::qNormalize(q);
	EXPECT_FLOAT_EQ(len, 0.f);
}

// ----- getRotationRate: shortest-arc wraparound at +/- PI -----

TEST_F(SpaceMathTest, RotationRateNoWrapInsidePi) {
	EXPECT_NEAR(SpaceMath::getRotationRate(1.f, 0.f), 1.f, EPS);
	EXPECT_NEAR(SpaceMath::getRotationRate(0.f, 1.f), -1.f, EPS);
}

TEST_F(SpaceMathTest, RotationRateWrapsPositiveOverPi) {
	// end - start = 1.5*PI should wrap to -0.5*PI (shorter the other way).
	float r = SpaceMath::getRotationRate(1.5f * M_PI, 0.f);
	EXPECT_NEAR(r, 1.5f * M_PI - 2.f * M_PI, EPS);
	EXPECT_LT(r, 0.f);
}

TEST_F(SpaceMathTest, RotationRateWrapsNegativeUnderPi) {
	float r = SpaceMath::getRotationRate(-1.5f * M_PI, 0.f);
	EXPECT_NEAR(r, -1.5f * M_PI + 2.f * M_PI, EPS);
	EXPECT_GT(r, 0.f);
}

// ----- velocity <-> rotation conversions are inverse on the canonical hemisphere -----

TEST_F(SpaceMathTest, RotationToVelocityProducesUnitVector) {
	Vector3 vel = SpaceMath::rotationToVelocity(Vector3(0.7f, 0.3f, 0.f));
	EXPECT_NEAR(SpaceMath::qSqrt(vel.squaredLength()), 1.f, EPS);
}

TEST_F(SpaceMathTest, VelocityRotationRoundTrip) {
	Vector3 rot(0.5f, 0.4f, 0.f);
	Vector3 vel = SpaceMath::rotationToVelocity(rot);
	Vector3 back = SpaceMath::velocityToRotation(vel);

	EXPECT_NEAR(back.getX(), rot.getX(), 1e-3f);
	EXPECT_NEAR(back.getY(), rot.getY(), 1e-3f);
}

// ----- getIntersection(SpaceTransform): lead/ray-segment intersection -----
// This is the "ray/segment" intersection target from the plan. The SpaceTransform
// overload is pure math (no appearance mesh) and runs headless.

TEST_F(SpaceMathTest, IntersectionDirectHitNormalized) {
	// Shooter at origin, firing straight down +X at unit speed over a distance of 10.
	// Target sits at (5,0,0) -> intersection fraction ~0.5 of the segment.
	SpaceTransform shooter;
	shooter.setPosition(Vector3(0.f, 0.f, 0.f));
	shooter.setVelocity(Vector3(1.f, 0.f, 0.f));

	float t = SpaceMath::getIntersection(shooter, Vector3(5.f, 0.f, 0.f), 10.f, 1.f);

	ASSERT_LT(t, FLT_MAX);
	EXPECT_NEAR(t, 0.5f, 1e-2f);
}

TEST_F(SpaceMathTest, IntersectionMissReturnsFltMax) {
	// Target far off the firing axis (well outside radius) -> miss.
	SpaceTransform shooter;
	shooter.setPosition(Vector3(0.f, 0.f, 0.f));
	shooter.setVelocity(Vector3(1.f, 0.f, 0.f));

	float t = SpaceMath::getIntersection(shooter, Vector3(5.f, 100.f, 0.f), 10.f, 1.f);
	EXPECT_FLOAT_EQ(t, FLT_MAX);
}

TEST_F(SpaceMathTest, IntersectionBehindShooterReturnsFltMax) {
	SpaceTransform shooter;
	shooter.setPosition(Vector3(0.f, 0.f, 0.f));
	shooter.setVelocity(Vector3(1.f, 0.f, 0.f));

	// Target behind the firing direction.
	float t = SpaceMath::getIntersection(shooter, Vector3(-5.f, 0.f, 0.f), 10.f, 1.f);
	EXPECT_FLOAT_EQ(t, FLT_MAX);
}

// ----- predictTransform: lead-prediction of where a contact will be -----
// The "predictTransform lead prediction" target. With zero angular delta the
// predicted position is a straight-line extrapolation: pos + velocity*speed*time.

TEST_F(SpaceMathTest, PredictTransformStraightLine) {
	SpaceTransform t;
	t.setPosition(Vector3(10.f, 0.f, 0.f));
	t.setVelocity(Vector3(1.f, 0.f, 0.f)); // unit heading +X
	t.setSpeed(50.f);
	t.setYprDelta(Vector3(0.f, 0.f, 0.f)); // no turn

	Vector3 predicted = SpaceMath::predictTransform(t, 2.f); // 2 seconds ahead

	// 10 + 1 * 50 * 2 = 110 along X.
	EXPECT_NEAR(predicted.getX(), 110.f, 1e-2f);
	EXPECT_NEAR(predicted.getY(), 0.f, 1e-2f);
	EXPECT_NEAR(predicted.getZ(), 0.f, 1e-2f);
}

TEST_F(SpaceMathTest, PredictTransformZeroTimeIsCurrentPosition) {
	SpaceTransform t;
	t.setPosition(Vector3(7.f, -3.f, 2.f));
	t.setVelocity(Vector3(1.f, 0.f, 0.f));
	t.setSpeed(100.f);

	Vector3 predicted = SpaceMath::predictTransform(t, 0.f);
	EXPECT_NEAR(predicted.getX(), 7.f, EPS);
	EXPECT_NEAR(predicted.getY(), -3.f, EPS);
	EXPECT_NEAR(predicted.getZ(), 2.f, EPS);
}

// ----- world<->local vector transform round-trip (Y/Z swap convention) -----
// "world-to-local" target. getLocalVector/getGlobalVector swap Y/Z and multiply by
// the rotation / conjugate matrix. With identity rotation they are exact inverses.

TEST_F(SpaceMathTest, LocalGlobalRoundTripIdentity) {
	Matrix4 identity; // default-constructed engine3 Matrix4 is identity

	Vector3 world(3.f, 5.f, 7.f);
	Vector3 local = SpaceMath::getLocalVector(world, identity);
	Vector3 back = SpaceMath::getGlobalVector(local, identity);

	EXPECT_NEAR(back.getX(), world.getX(), EPS);
	EXPECT_NEAR(back.getY(), world.getY(), EPS);
	EXPECT_NEAR(back.getZ(), world.getZ(), EPS);
}

// ----- rotationToQuaternion(precision=true): produces a normalized quaternion -----
// precision=true does NOT touch RotationLookupTable, so it is headless-safe.

TEST_F(SpaceMathTest, RotationToQuaternionIsNormalized) {
	Quaternion q = SpaceMath::rotationToQuaternion(Vector3(0.6f, 0.2f, 0.1f), true);
	EXPECT_NEAR(SpaceMath::qSqrt(q.lengthSquared()), 1.f, EPS);
}

TEST_F(SpaceMathTest, RotationToQuaternionZeroRotationIsValidUnit) {
	Quaternion q = SpaceMath::rotationToQuaternion(Vector3(0.f, 0.f, 0.f), true);
	EXPECT_NEAR(SpaceMath::qSqrt(q.lengthSquared()), 1.f, EPS);
}

} // namespace test
} // namespace transform
} // namespace ship
} // namespace objects
} // namespace zone
} // namespace server
