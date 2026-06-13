/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#include "server/zone/objects/intangible/IntangibleObject.h"
#include "server/zone/objects/intangible/TheaterObject.h"
#include "server/zone/packets/scene/AttributeListMessage.h"

#include "server/zone/packets/intangible/IntangibleObjectMessage3.h"
#include "server/zone/packets/intangible/IntangibleObjectMessage6.h"
#include "server/zone/packets/intangible/IntangibleObjectDeltaMessage3.h"
#include "server/zone/packets/scene/IsFlattenedTheaterMessage.h"

void IntangibleObjectImplementation::initializeTransientMembers() {
	SceneObjectImplementation::initializeTransientMembers();

	setLoggingName("IntangibleObject");
}

void IntangibleObjectImplementation::sendBaselinesTo(SceneObject* player) {
	debug("sending intangible object baselines");

	BaseMessage* itno3 = new IntangibleObjectMessage3(_this.getReferenceUnsafeStaticCast());
	player->sendMessage(itno3);

	BaseMessage* itno6 = new IntangibleObjectMessage6(_this.getReferenceUnsafeStaticCast());
	player->sendMessage(itno6);

	if (isTheaterObject()) {
		TheaterObject* theater = cast<TheaterObject*>(_this.getReferenceUnsafeStaticCast());

		if (theater != nullptr && theater->shouldFlattenTheater()) {
			BaseMessage* ift = new IsFlattenedTheaterMessage(getObjectID(), true);
			player->sendMessage(ift);
		}
	}
}

void IntangibleObjectImplementation::updateStatus(int newStatus, bool notifyClient) {
	if (status == newStatus)
		return;

	status = newStatus;

	if (!notifyClient)
		return;

	ManagedReference<SceneObject*> strongParent = getParent().get();
	if (strongParent == nullptr)
		return;

	ManagedReference<SceneObject*> player = strongParent->getParent().get();

	if (player == nullptr)
		return;

	IntangibleObjectDeltaMessage3* delta = new IntangibleObjectDeltaMessage3(_this.getReferenceUnsafeStaticCast());
	delta->updateStatus(newStatus);
	delta->close();
	player->sendMessage(delta);
}

void IntangibleObjectImplementation::setCustomObjectName(const UnicodeString& name, bool notifyClient) {
	customName = name;

	if (!notifyClient)
		return;

	IntangibleObjectDeltaMessage3* ditno3 = new IntangibleObjectDeltaMessage3(_this.getReferenceUnsafeStaticCast());
	ditno3->updateName(name);
	ditno3->close();

	broadcastMessage(ditno3, true);
}

// ===== P2: ported missing method implementations (link-stage undefined symbols) =====
// Droid-command chip attribute display + setters. Delta-message broadcast classes for these
// fields are not present in this tree, so setters update state without a client delta (acceptable;
// values are sent on next full update).
void IntangibleObjectImplementation::fillAttributeList(AttributeListMessage* alm, CreatureObject* player) {
	if (datapadSize > 0) {
		alm->insertAttribute("droid_command_program_size", datapadSize);
	}

	if (!itemIdentifier.isEmpty()) {
		alm->insertAttribute("droid_command_name", itemIdentifier);
	}
}

void IntangibleObjectImplementation::setDataSize(const float dataSize, bool notifyClient) {
	datapadSize = dataSize;
}

void IntangibleObjectImplementation::setItemIdentifier(const String& itemName, bool notifyClient) {
	itemIdentifier = itemName;
}
