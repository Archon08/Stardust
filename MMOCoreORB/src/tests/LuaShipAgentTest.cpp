// PORTED VERBATIM (Phase 6.A) from swgemu/Core3 @ 6856f315a80b5250635b2272695caec1d64204ed
// Path in pin: MMOCoreORB/src/tests/LuaShipAgentTest.cpp
// NOTE: requires server config/DB and (for LuaShipAgentTest) Lua datatables + TRE; not part
// of the headless GREEN gate. Run via the non-fatal broad step in space-tests.yml.
/*
 * LuaShipAgentTest.cpp
 *
 * Created on: 2024/11/1
 * Author: Hakry
 */

#include "gtest/gtest.h"
#include "server/zone/managers/ship/ShipAgentTemplateManager.h"

class LuaShipAgentTest : public ::testing::Test {

public:
	LuaShipAgentTest() {
	}

	~LuaShipAgentTest() {
	}

	void SetUp() {
	}

	void TearDown() {
	}
};

TEST_F(LuaShipAgentTest, LuaShipAgentTemplateTest) {
	ShipAgentTemplateManager::DEBUG_MODE = 1;

	// Verify that all ship agents load
	ASSERT_EQ(ShipAgentTemplateManager::instance()->loadTemplates(), 0);
}