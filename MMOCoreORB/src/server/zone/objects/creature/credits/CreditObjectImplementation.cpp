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
// ===== P2: ported from upstream (link-stage undefined symbols; CreatureObject credit methods depend on these) =====
void CreditObjectImplementation::transferCredits(int cash, int bank, bool notifyClient) {
	if (cash < 0 || bank < 0 || cash > CreditObject::CREDITCAP || bank > CreditObject::CREDITCAP) {
		return;
	}

	if ((uint32) cashCredits + (uint32) bankCredits != (uint32) cash + (uint32) bank) {
		return;
	}

	setCashCredits(cash, notifyClient);
	setBankCredits(bank, notifyClient);
}

bool CreditObjectImplementation::subtractCredits(int credits, bool notifyClient, bool bankFirst) {
	if (credits < 0) {
		return false;
	}

	if (credits > cashCredits + bankCredits) {
		return false;
	}

	if (bankFirst) {
		if (bankCredits > credits) {
			subtractBankCredits(credits, notifyClient);
		} else {
			credits -= bankCredits;
			clearBankCredits(notifyClient);
			subtractCashCredits(credits, notifyClient);
		}
	} else {
		if (cashCredits > credits) {
			subtractCashCredits(credits, notifyClient);
		} else {
			credits -= cashCredits;
			clearCashCredits(notifyClient);
			subtractBankCredits(credits, notifyClient);
		}
	}

	return true;
}
