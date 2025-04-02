#pragma once
#include "baseTypes.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct leveldef_t {
    Bounds2D fieldBounds;
    uint32_t fieldColor;
    uint32_t numBalls;
    uint32_t numFaces;
} LevelDef;

typedef struct level_t Level;

void levelMgrInit();
void levelMgrShutdown();
Level *levelMgrLoad(const LevelDef* levelDef);
void levelMgrUnload(Level* level);

void spawnSoldier(Level* level, int32_t soliderType, bool reverse);

void handleMouseClick(Level* level, Coord2D mousePosition);

void incLevelExp();

int getPlayerArmyStr();

void decPlayerArmyStr(int sdType);
void updateLevelExpUI(Level* curLevel);

void updateGold(int32_t gold);

float playerBaseHp(Level* curLevel);

float AIBaseHp(Level* curLevel);

void initGameOverScreen();

#ifdef __cplusplus
}
#endif