/*
 * LootBoxesMenucomponent.cpp
 *
 *  Created on: 01/25/2012
 *      Author: Elvaron
 */

#include "server/zone/objects/creature/CreatureObject.h"
#include "LootBoxesMenucomponent.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/managers/director/DirectorManager.h"
#include "server/zone/managers/loot/LootManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"

void LootBoxesMenucomponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) const {
	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

}

int LootBoxesMenucomponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) const {
	if (!sceneObject->isTangibleObject())
		return 0;

	if (!player->isPlayerCreature())
		return 0;

	if (selectedID == 20) {
		if (!sceneObject->isASubChildOf(player)) {
			player->sendSystemMessage("You can only claim a loot box when it is in your inventory.");
			return 0;
		}

		ManagedReference<SceneObject*> inventory = player->getSlottedObject("inventory");

		if (inventory == NULL || inventory->isContainerFullRecursive()) {
			player->sendSystemMessage("Your inventory is full, so the item could not be created.");
			return 0;
		}

		ManagedReference<LootManager*> lootManager = sceneObject->getZone()->getZoneServer()->getLootManager();

		if (lootManager == NULL)
			return 0;

		String fullTemplate = sceneObject->getObjectTemplate()->getFullTemplateString();

		if (fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox.iff" || fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s1.iff" || fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s2.iff" || fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s3.iff") {

			lootManager->createLoot(inventory, "one_year_paintings", 1);
			lootManager->createLoot(inventory, "g_event", 250);
			sceneObject->destroyObjectFromWorld(true);
			sceneObject->destroyObjectFromDatabase(true);

			String containerTemplate = "";

			if (fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox.iff")
				containerTemplate = "object/tangible/container/holiday/anniversary/one_year_lootbox_container.iff";
			else if (fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s1.iff")
				containerTemplate = "object/tangible/container/holiday/anniversary/one_year_lootbox_container_s1.iff";
			else if (fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s2.iff")
				containerTemplate = "object/tangible/container/holiday/anniversary/one_year_lootbox_container_s2.iff";
			else if (fullTemplate == "object/tangible/holiday/anniversary/one_year_lootbox_s3.iff")
				containerTemplate = "object/tangible/container/holiday/anniversary/one_year_lootbox_container_s3.iff";

			ManagedReference<TangibleObject*> oneYearContainer = player->getZoneServer()->createObject(containerTemplate.hashCode(), 1).castTo<TangibleObject*>();

			if (oneYearContainer == NULL) {
				return 0;
			}

			Locker oneYearContainerLocker(oneYearContainer);

			inventory->transferObject(oneYearContainer, -1, true);

			oneYearContainer->sendTo(player, true);

			DirectorManager::instance()->startScreenPlay(player, "oneYearLootboxScreenplay");
		} else {
			player->sendSystemMessage("You were unable to claim the loot box.");
		}

		return 0;
	}

	return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
}
