#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Object.h"
#include "AIcontroller.h"
#include "levelmgr.h"
#include "time.h"
#include "random.h"

static int32_t timeElapsed = 0;

static int armyIndex[] = { 3,4,5 };
static int armyStrengthRating[] = { 8, 10, 24 };
static int AIArmyStr = 0;
// when player army strength is below this level, ai should match this level
static int idleArmyStrength = 40;

/*
* AI spawn soldier
*/
void AI_SpawnSoldier(Level* level) {
	int playerStr = getPlayerArmyStr();

	// 1-5 ai decision
	int AI_decision = randGetInt(1, 5);
	if (playerStr <= idleArmyStrength) {
		// if ai army exceed idleStr, return
		if (AIArmyStr >= idleArmyStrength) {
			return;
		}
		else {
			// higher chance of spawning first type
			if (AI_decision == 1 || AI_decision == 5) {
				spawnSoldier(level, armyIndex[0], true);
				AIArmyStr += armyStrengthRating[0];
			}
			else if (AI_decision == 2) {
				spawnSoldier(level, armyIndex[1], true);
				AIArmyStr += armyStrengthRating[1];
			}
			else if (AI_decision == 3){
				spawnSoldier(level, armyIndex[2], true);
				AIArmyStr += armyStrengthRating[2];
			}
			else {
				// ai_decision 4 skips spawn
				return;
			}
		}
		
	}
	else {
		// player have sizable army
		if (AIArmyStr < playerStr) {
			// AI reaches army strength 70, stop spawn
			if (AIArmyStr >= 70) {
				return;
			}
			else {
				// higher chance of spawning STRONGEST unit
				if (AI_decision == 1 || AI_decision == 2) {
					spawnSoldier(level, armyIndex[2], true);
					AIArmyStr += armyStrengthRating[2];
				}
				else if (AI_decision == 3 || AI_decision == 4) {
					spawnSoldier(level, armyIndex[1], true);
					AIArmyStr += armyStrengthRating[1];
				}
				else if (AI_decision == 5) {
					spawnSoldier(level, armyIndex[0], true);
					AIArmyStr += armyStrengthRating[0];
				}
			}
			
		}
		else {
			if (AIArmyStr >= 80) {
				return;
			}
		}
	}

};
void AI_Update(Level* level, uint32_t milliseconds) {
	timeElapsed += milliseconds;
	// spawn soldier every 4 seconds
	if (timeElapsed >= 4000) {
		AI_SpawnSoldier(level);
		timeElapsed = 0;
	}
}
// decrease AI army strength
void decAIArmyStr(int sdType) {
	assert(sdType > 2);
	AIArmyStr -= armyStrengthRating[sdType - 3];
}
