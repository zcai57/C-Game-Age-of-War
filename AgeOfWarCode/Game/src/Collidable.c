#include "Soldier.h"
#include "baseTypes.h"
#include "Collidable.h"
#include "object.h"
#include <stdlib.h>
// This is so poorly designed, collidable should not be soldier only!
// A soldier table for all soldiers
static Soldier** soldierCollidable;
// max amount of collidable soldiers
static int32_t maxCollidable = 30;
/*
* Init collidable
*/
void collidable_Init() {
	soldierCollidable = malloc(maxCollidable * sizeof(Soldier*));
	for (int i = 0; i < maxCollidable; i++) {
		soldierCollidable[i] = NULL;
	}
}
/*
* Add soldiers into collidable
*/
int32_t add_collidable(Soldier* sd) {
	int index = 0;
	for (int i = 0; i < maxCollidable; i++) {
		if (soldierCollidable[i] == NULL) {
			soldierCollidable[i] = sd;
			index = i;
			break;
		}
	}
	return index;
}
/*
* Remove collidable based on index
*/
void remove_collidable(int32_t num) {
	soldierCollidable[num] = NULL;
}
/*
* Get collidable reference
*/
Soldier** getCollidable() {
	return soldierCollidable;
}
/*
* Get max number of collidable
*/
int32_t getMax()
{
	return maxCollidable;
}
