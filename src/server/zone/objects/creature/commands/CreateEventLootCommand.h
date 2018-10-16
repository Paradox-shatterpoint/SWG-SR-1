/*
		Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CreateEventLootCOMMAND_H_
#define CreateEventLootCOMMAND_H_

#include "server/zone/managers/loot/LootManager.h"
#include "server/zone/objects/scene/SceneObject.h"

class CreateEventLootCommand : public QueueCommand {
public:

	CreateEventLootCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		StringTokenizer args(arguments.toString());
		try {
			String lootGroup = "g_event"; //Change this to set event lootgroup, 
			
			int range = 64;
			if (args.hasMoreTokens())
				range = args.getIntToken();

			if( range < 0 )
				range = 32;

			if( range > 128 )
				range = 128;

			int level = 250;
			if (args.hasMoreTokens())
				level = args.getIntToken();

			ManagedReference<LootManager*> lootManager = creature->getZoneServer()->getLootManager();
			if (lootManager == NULL)
				return INVALIDPARAMETERS;

			Zone* zone = creature->getZone();
			if (zone == NULL)
				return GENERALERROR;

			// Find all objects in range
			SortedVector<QuadTreeEntry*> closeObjects;
			CloseObjectsVector* closeObjectsVector = (CloseObjectsVector*) creature->getCloseObjects();
			if (closeObjectsVector == NULL) {
				zone->getInRangeObjects(creature->getPositionX(), creature->getPositionY(), range, &closeObjects, true);
			} else {
				closeObjectsVector->safeCopyTo(closeObjects);
			}

			// Award loot group to all players in range
			for (int i = 0; i < closeObjects.size(); i++) {
				SceneObject* targetObject = static_cast<SceneObject*>(closeObjects.get(i));

				if (targetObject->isPlayerCreature() && creature->isInRange(targetObject, range)) {

					CreatureObject* targetPlayer = cast<CreatureObject*>(targetObject);
					Locker tlock( targetPlayer, creature );

					ManagedReference<SceneObject*> inventory = targetPlayer->getSlottedObject("inventory");
					if (inventory != NULL) {
						if( lootManager->createLoot(inventory, lootGroup, level) )
					targetPlayer->sendSystemMessage( "You have received a loot item!");
					}

					tlock.release();
				}
			}
		} catch (Exception& e) {
			creature->sendSystemMessage("SYNTAX: /createEventLoot [<range>] [<level>]");

			return INVALIDPARAMETERS;
		}
		return SUCCESS;
	}

};

#endif //CreateEventLootCOMMAND_H_
