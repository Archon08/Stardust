/*
 * ActiveAreaImplementation.cpp
 *
 *  Created on: 02/06/2010
 *      Author: victor
 */

#include "server/zone/objects/area/ActiveArea.h"
#include "events/ActiveAreaEvent.h"
#include "server/zone/objects/area/areashapes/AreaShape.h"
#include "server/zone/objects/area/areashapes/RectangularAreaShape.h"

bool ActiveAreaImplementation::containsPoint(float px, float py, uint64 cellid) {
	if (cellObjectID != 0 && cellObjectID != cellid)
		return false;

	return containsPoint(px, py);
}

bool ActiveAreaImplementation::containsPoint(float px, float py) {
	if (areaShape == nullptr) {
		return QuadTreeEntryImplementation::containsPoint(px, py);
	}

	return areaShape->containsPoint(px, py);
}

bool ActiveAreaImplementation::containsPoint(float px, float pz, float py, uint64 cellid) {
	if (cellObjectID != 0 && cellObjectID != cellid)
		return false;

	return containsPoint(px, pz, py);
}

bool ActiveAreaImplementation::containsPoint(float px, float pz, float py) {
	if (areaShape == nullptr) {
		float dx = getPositionX() - px;
		float dy = getPositionY() - py;
		float dz = getPositionZ() - pz;

		return (dx * dx + dy * dy + dz * dz) <= getRadius2();
	}

	return areaShape->containsPoint(Vector3(px, pz, py));
}

void ActiveAreaImplementation::enqueueEnterEvent(SceneObject* obj) {
#ifdef WITH_STM
	notifyEnter(obj);
#else
	Reference<Task*> task = new ActiveAreaEvent(_this.getReferenceUnsafeStaticCast(), obj, ActiveAreaEvent::ENTEREVENT);
	obj->executeOrderedTask(task);

#endif
}

void ActiveAreaImplementation::enqueueExitEvent(SceneObject* obj) {
#ifdef WITH_STM
	notifyExit(obj);
#else
	Reference<Task*> task = new ActiveAreaEvent(_this.getReferenceUnsafeStaticCast(), obj, ActiveAreaEvent::EXITEVENT);
	obj->executeOrderedTask(task);

#endif
}

void ActiveAreaImplementation::notifyEnter(SceneObject* obj) {
	if (cellObjectID == 0 || cellObjectID == obj->getParentID())
		notifyObservers(ObserverEventType::ENTEREDAREA, obj);

	if (obj->isPlayerCreature() && attachedScenery.size() > 0) {
		ManagedReference<SceneObject*> sceno = obj;
		Vector<ManagedReference<SceneObject* > > scene = attachedScenery;

		Core::getTaskManager()->executeTask([=] () {
			for (int i = 0; i < scene.size(); i++) {
				SceneObject* scenery = scene.get(i);
				Locker locker(scenery);

				scenery->sendTo(sceno, true);
			}
		}, "SendSceneryLambda");
	}
}

void ActiveAreaImplementation::notifyExit(SceneObject* obj) {
	if (cellObjectID == 0 || cellObjectID != obj->getParentID())
		notifyObservers(ObserverEventType::EXITEDAREA, obj);

	if (obj->isPlayerCreature() && attachedScenery.size() > 0) {
		ManagedReference<SceneObject*> sceno = obj;
		Vector<ManagedReference<SceneObject* > > scene = attachedScenery;

		Core::getTaskManager()->executeTask([=] () {
			for (int i = 0; i < scene.size(); i++) {
				SceneObject* scenery = scene.get(i);
				Locker locker(scenery);

				scenery->sendDestroyTo(sceno);
			}
		}, "SendDestroySceneryLambda");
	}
}

void ActiveAreaImplementation::setZone(Zone* zone) {
	this->zone = zone;
}

bool ActiveAreaImplementation::intersectsWith(ActiveArea* area) {
	if (areaShape == nullptr) {
		return false;
	}

	return areaShape->intersectsWith(area->getAreaShape());
}

void ActiveAreaImplementation::initializeChildObject(SceneObject* controllerObject) {
	ManagedReference<SceneObject*> objectParent = controllerObject->getParent().get();

	if (objectParent != nullptr && objectParent->isCellObject()) {
		setCellObjectID(objectParent->getObjectID());
	}
}

Vector3 ActiveAreaImplementation::getAreaCenter() const {
	if (areaShape != nullptr)
		return areaShape->getAreaCenter();
	else
		return getPosition();
}

float ActiveAreaImplementation::getWidth() const {
	if (areaShape != nullptr) {
		RectangularAreaShape* rect = dynamic_cast<RectangularAreaShape*>(areaShape.get());

		if (rect != nullptr)
			return rect->getWidth();
	}

	return 0.f;
}

float ActiveAreaImplementation::getHeight() const {
	if (areaShape != nullptr) {
		RectangularAreaShape* rect = dynamic_cast<RectangularAreaShape*>(areaShape.get());

		if (rect != nullptr)
			return rect->getHeight();
	}

	return 0.f;
}

Vector4 ActiveAreaImplementation::getRectangularDimensions() const {
	Vector3 center = getAreaCenter();

	float halfWidth = getWidth() * 0.5f;
	float halfHeight = getHeight() * 0.5f;

	if (halfWidth == 0.f && halfHeight == 0.f) {
		float r = getRadius();
		halfWidth = r;
		halfHeight = r;
	}

	// x,y = lower-left; z,w = upper-right
	return Vector4(center.getX() - halfWidth, center.getY() - halfHeight, center.getX() + halfWidth, center.getY() + halfHeight);
}

Vector3 ActiveAreaImplementation::getCuboidDimensions() const {
	float r = getRadius();

	return Vector3(r, r, r);
}
