/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SPAWNSTATUSCOMMAND_H_
#define SPAWNSTATUSCOMMAND_H_

#include "server/zone/objects/creature/ai/AiAgent.h"
#include "server/zone/objects/creature/CreatureObject.h"

class SpawnStatusCommand : public QueueCommand {
public:

	SpawnStatusCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		ManagedReference<AiAgent*> targetObj = server->getZoneServer()->getObject(creature->getTargetID()).castTo<AiAgent*>();
		if (targetObj == nullptr)
			return GENERALERROR;

		// outputLuaTimes was removed from AiAgent in the modern engine; report basic status instead.
		StringBuffer status;
		status << "Spawn status for " << targetObj->getDisplayedName()
			<< " - movementState: " << targetObj->getMovementState();
		creature->sendSystemMessage(status.toString());

		return SUCCESS;
	}

};

#endif //SPAWNSTATUSCOMMAND_H_
