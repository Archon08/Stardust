#include "server/zone/objects/creature/credits/CreditObject.h"
#include "server/zone/packets/DeltaMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"

void CreditObjectImplementation::setCashCredits(int credits, bool notifyClient) {
	if (cashCredits == credits)
		return;

	assert(credits >= 0);

	cashCredits = credits;

	if (notifyClient) {
		Reference<CreatureObject*> creo = owner.get();
		if (creo == nullptr)
			return;

		DeltaMessage *msg = new DeltaMessage(creo->getObjectID(), 'CREO', 1);
		msg->startUpdate(0x01);
		msg->insertInt(cashCredits);
		msg->close();
		creo->sendMessage(msg);
	}
}

WeakReference<CreatureObject*> CreditObjectImplementation::getOwner() {
	return owner;
}

uint64 CreditObjectImplementation::getOwnerObjectID() {
	Reference<CreatureObject*> ownerRef = owner.get();

	return ownerRef != nullptr ? ownerRef->getObjectID() : 0;
}

void CreditObjectImplementation::setOwner(CreatureObject* obj) {
	owner = obj;
}

void CreditObjectImplementation::setBankCredits(int credits, bool notifyClient) {
	if (bankCredits == credits)
		return;

	assert(credits >= 0);

	bankCredits = credits;

	if (notifyClient) {
		Reference<CreatureObject*> creo = owner.get();
		if (creo == nullptr)
			return;

		DeltaMessage *msg = new DeltaMessage(creo->getObjectID(), 'CREO', 1);
		msg->startUpdate(0x00);
		msg->insertInt(bankCredits);
		msg->close();
		creo->sendMessage(msg);
	}
}

void CreditObjectImplementation::subtractBankCredits(int credits, bool notifyClient) {
	setBankCredits(bankCredits-credits, notifyClient);
}

void CreditObjectImplementation::subtractCashCredits(int credits, bool notifyClient) {
	setCashCredits(cashCredits - credits, notifyClient);
}

void CreditObjectImplementation::notifyLoadFromDatabase() {
	ManagedObjectImplementation::notifyLoadFromDatabase();
	if (cashCredits < 0)
		cashCredits = 0;

	if (bankCredits < 0)
		bankCredits = 0;
}