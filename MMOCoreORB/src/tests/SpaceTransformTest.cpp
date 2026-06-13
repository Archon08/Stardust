/*
 * SpaceTransformTest.cpp
 *
 * Phase 6 (verification) headless unit tests for the SpaceTransform value type used
 * throughout the ship transform / interpolation pipeline. Pure value object; no
 * server, DB, TRE or Lua dependency -- runs entirely in CI.
 */

#include "gtest/gtest.h"

#include "server/zone/objects/ship/transform/SpaceTransform.h"

namespace server {
namespace zone {
namespace objects {
namespace ship {
namespace transform {
namespace test {

class SpaceTransformTest : public ::testing::Test {
public:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(SpaceTransformTest, DefaultConstructionZeroesSpeed) {
	SpaceTransform t;
	EXPECT_FLOAT_EQ(t.getSpeed(), 0.f);
}

TEST_F(SpaceTransformTest, DefaultVectorsAreZero) {
	SpaceTransform t;
	EXPECT_FLOAT_EQ(t.getPosition().getX(), 0.f);
	EXPECT_FLOAT_EQ(t.getPosition().getY(), 0.f);
	EXPECT_FLOAT_EQ(t.getPosition().getZ(), 0.f);
	EXPECT_FLOAT_EQ(t.getVelocity().squaredLength(), 0.f);
	EXPECT_FLOAT_EQ(t.getRotation().squaredLength(), 0.f);
	EXPECT_FLOAT_EQ(t.getYprDelta().squaredLength(), 0.f);
}

TEST_F(SpaceTransformTest, SettersRoundTrip) {
	SpaceTransform t;
	t.setPosition(Vector3(1.f, 2.f, 3.f));
	t.setRotation(Vector3(0.1f, 0.2f, 0.3f));
	t.setYprDelta(Vector3(0.4f, 0.5f, 0.6f));
	t.setVelocity(Vector3(1.f, 0.f, 0.f));
	t.setSpeed(123.f);

	EXPECT_FLOAT_EQ(t.getPosition().getX(), 1.f);
	EXPECT_FLOAT_EQ(t.getPosition().getY(), 2.f);
	EXPECT_FLOAT_EQ(t.getPosition().getZ(), 3.f);

	EXPECT_FLOAT_EQ(t.getRotation().getX(), 0.1f);
	EXPECT_FLOAT_EQ(t.getYprDelta().getZ(), 0.6f);
	EXPECT_FLOAT_EQ(t.getVelocity().getX(), 1.f);
	EXPECT_FLOAT_EQ(t.getSpeed(), 123.f);
}

TEST_F(SpaceTransformTest, DebugStringIsNonEmpty) {
	SpaceTransform t;
	t.setSpeed(10.f);
	EXPECT_GT(t.toDebugString().length(), (int) 0);
}

} // namespace test
} // namespace transform
} // namespace ship
} // namespace objects
} // namespace zone
} // namespace server
