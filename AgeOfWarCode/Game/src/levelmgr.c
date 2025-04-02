#include <Windows.h>
#include <stdlib.h>
#include <gl/GLU.h>
#include <assert.h>
#include "baseTypes.h"
#include "levelmgr.h"
#include "objmgr.h"
#include "SOIL.h"
#include "sound.h"
#include "shape.h" 
#include "background.h"
#include "Soldier.h"
#include "Collidable.h"
#include "UIButton.h"
#include "UI_Bar.h"
#include "numberRender.h"

typedef struct level_t
{
    const LevelDef* def;
    // background component
    Background* bg;
    // soldier component
    Soldier* sd;
    // exp ui bar component
    UIBar* exp;
    base* playerBase;
    base* AIBase;
    // number renderer
    display* numRender;
} Level;
// level number
static int32_t LevelNum;
// cur level reference
static Level* curLevel;

static int32_t playerGold;
// Base hp 
static float baseHpIncrement = 400.0f;
// player and AI base pos
static Coord2D playerBasePos = { -110.0f, 315.0f };
static Coord2D AIBasePos = { 1210.0f, 315.0f };
// soldier price table
static int32_t soldier_price[3] = { 20, 30, 80 };
// experience counter
static float Exp;
// max exp for each era upgrade
static float maxExp;
// player army strength counter
static int playerArmyStrength = 0;
// unit strength rating
static int armyStrengthRating[] = { 8, 10, 24 };

static int32_t _soundId = SOUND_NOSOUND;
/// @brief Initialize the level manager
/*
* Init level manager
*/
void levelMgrInit()
{
    // init backgound texture
    backgroundInitTexture();
    // init level num
    LevelNum = 0;
    // init background music
    _soundId = soundLoad("asset/background.wav");
    // play background music
    soundPlay(_soundId);
    // init collidable array
    collidable_Init();
    // init xp and player gold
    Exp = 0;
    maxExp = 50;
    playerGold = 100;
}

/// @brief Shutdown the level manager
void levelMgrShutdown()
{
   /* ballClearCollideCB();*/
    soundUnload(_soundId);
}
/*
* Exp increment
*/
void incLevelExp() {
    Exp += 1.0f;
}
/*
* Update EXP ui
*/
void updateLevelExpUI(Level* curLevel) {
    if (Exp > maxExp) {
        return;
    }
    curLevel->exp->percent = Exp / maxExp;
}
/*
* Getter function for player base hp
*/
float playerBaseHp(Level* curLevel) {
    return curLevel->playerBase->hp;
}
/*
* Getter function for AI base hp
*/
float AIBaseHp(Level* curLevel) {
    return curLevel->AIBase->hp;
}
/*
* Display game over screen
*/
void initGameOverScreen() {
    button_init((Coord2D) { 300.0f, 0.0f }, "asset/gameover.png", 900, 640);
}
/*
* Update player gold
*/
void updateGold(int32_t gold) {
    playerGold += gold;
    assert(curLevel->numRender != NULL);
    numRender_update(curLevel->numRender, playerGold);
}
/*
* Spawn soldier
*/
void spawnSoldier(Level* level, int soldierType, bool reverse) {
    // init soldier speed
    float speed = 0.3f;
    if (reverse) {
        speed = -0.3f;
    }
    // if not AI, add player Army strength
    else {
        assert(soldierType < 3);
        playerArmyStrength += armyStrengthRating[soldierType];
    }
    // Forgot what this is for
    level->sd = malloc(sizeof(Soldier*));
    if (level->sd != NULL) {
        if (reverse) {
            // spawn AI soldier
            sdNew(LevelNum, soldierType, speed, reverse, level->playerBase);
        }
        else {
            // calculate soldier price
            int32_t sdPrice = soldier_price[soldierType] * (LevelNum + 1);
            // If player have enough gold, spawn player soldier
            if (playerGold >= sdPrice) {
                sdNew(LevelNum, soldierType, speed, reverse, level->AIBase);
                updateGold(-sdPrice);
            }
        }
    }
}
/*
* Upgrade button mouse click action
*/
void handleMouseClick(Level* level, Coord2D mousePosition) {
    // Better not do hard coded locations! But alas..
    // When mouse clicked at button location and exp reaches max
    if (mousePosition.x > 1150.0 && mousePosition.x < 1184.0f &&
        mousePosition.y > 23.0f && mousePosition.y < 57.0f && Exp >= maxExp) {
        // if not at era 2, increment era
        if (LevelNum != 2) {
            LevelNum += 1;
            maxExp *= 1.5;
            Exp = 0;

            assert(level->playerBase != NULL);
            assert(level->AIBase != NULL);
            // new base for player and AI
            float pHP = level->playerBase->hp;
            float aHP = level->AIBase->hp;
            // change player base hp and texture
            level->playerBase->maxHp = baseHpIncrement * (LevelNum + 1);
            level->AIBase->maxHp = baseHpIncrement * (LevelNum + 1);
            level->playerBase->hp = pHP + baseHpIncrement;
            level->AIBase->hp = aHP + baseHpIncrement;
            level->playerBase->_baseTexture = 0;
            baseInitTexture(level->playerBase, LevelNum + 1);
            level->AIBase->_baseTexture = 0;
            baseInitTexture(level->AIBase, LevelNum + 1);
        }
    }
}

/*
* Getter function for player army strength
*/
int getPlayerArmyStr() {
    return playerArmyStrength;
}
/*
* Decrease player army strength
*/
void decPlayerArmyStr(int sdType) {
    assert(sdType < 3);
    assert(playerArmyStrength >= armyStrengthRating[sdType]);
    playerArmyStrength -= armyStrengthRating[sdType];
}
/// @brief Loads the level and all required objects/assets
/// @param levelDef 
/// @return 
Level* levelMgrLoad(const LevelDef* levelDef)
{
    Level* level = malloc(sizeof(Level));
    curLevel = level;
    if (level != NULL)
    {
        level->def = levelDef;
        LevelNum = 0;
        // Init background
        level->bg = malloc(sizeof(Background*));
        if (level->bg != NULL) {
            level->bg = bgNew();
        }

        // init buttons
        Coord2D upgradeBtPos = { 1150.0f, 23.0f };
        button_init(upgradeBtPos, "asset/Upgrade.png", 34, 34);

        // init exp ui bar
        level->exp = init_UIbar(800.0f, 30.0f, 255, 255, 255, 0, 0, 255,
            300.0f, 20.0f, true);
        level->exp->percent = 0.0f;

        // init base
        level->playerBase = base_init(playerBasePos, baseHpIncrement, LevelNum+1, false);
        level->AIBase = base_init(AIBasePos, baseHpIncrement, LevelNum+1, true);
        
        // init gold ui icon
        button_init((Coord2D) { 230.0f, 15.f }, "asset/Gold.png", 68, 68);
        // init number renderer
        level->numRender = numRender_init(playerGold, (Coord2D){300.0f, 30.f}, 8);
        
    }
    return level;
}


/// @brief Unloads the level and frees up any assets associated
/// @param level 
void levelMgrUnload(Level* level)
{
    if (level != NULL) 
    {
        // forgot to implement this ?_?
    }
    free(level);
}
