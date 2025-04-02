#include "Soldier.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "Object.h"
#include "SOIL.h"
#include "input.h"
#include "UI_Bar.h"
#include "Collidable.h"
#include "baseTypes.h"
#include "Projectile.h"
#include "levelmgr.h"
#include "AIcontroller.h"


typedef struct soldier_t {
	Object obj;
	int type;
    // walk frame
    int wkFrame;
    // attack frame
    int atFrame;
    // death frame
    int dFrame;
    // idle frame
    int iFrame;
    // uv of the texture
    int32_t* sdUV;
    // soldier name
    char* sdName;
    // animation state
    int32_t animState;
    // glu texture
    GLuint _sdTexture;
    // frame counter to control animation speed
    int32_t frameCounter;
    // player or enemy (reverse x render or not)
    boolean reverse;
    // index in collidable array
    int32_t collidable_index;
    // soldier attack range
    int32_t atkRange;
    // soldier attack power
    int32_t atkPower;
    float initHp;
    // current hp
    float Hp;
    // health bar ui component
    UIBar* healthBar;
    // attack target
    Object* atkTarget;
    int level; // 0,1,2
    // enemy base component
    base* enemyBase;
    int projectile_type; // -1, 0, 1, 2, 3
    float speed;
} Soldier; 
// Array of solider texture - solidername, uv, {row of atk animation, frame of attack animation}
const sdTexture soldierType[3][6][20] = {
    {
        {"Soldier.png",{9,7}, {3,6}},
        {"Archer.png", {12,6}, {3,9}},
        {"Armored Axeman.png", {12,7}, {3,9}},
        {"Slime.png", {12,6}, {3, 6}},
        {"Werewolf.png", {13,6}, {3, 9}},
        {"Werebear.png", {13,7}, {3, 9}},
    },
    {
        {"Swordsman.png", {15, 7}, {3, 7}},
        {"Wizard.png", {15,10}, {7, 6}},
        {"Lancer.png", {9, 8}, {4, 6}},
        {"Orc.png", {8,6}, {3, 6}},
        {"Orc rider.png", {11, 8}, {4, 7}},
        {"Elite Orc.png", {11, 7}, {3, 7}},
    },
    {
        {"Knight.png", {11, 8}, {3, 7}},
        {"Priest.png", {9, 10}, {6, 6}},
        {"Knight Templar.png", {11, 9}, {5, 8}},
        {"Skeleton.png", {8, 7}, {3, 6}},
        {"Skeleton Archer.png", {9,5}, {3, 9}},
        {"Skeleton GS.png", {12, 7}, {3, 9}},
     },
};
// enemy death gold reward, tweaked
static int32_t soldier_price[3] = { 15, 20, 70 };
// enemy death gold reward mutiplier
static float enemyPrice_offset = 1.2f;
// fast unit speed offset
static float speedOffset = 0.3f;
// starting x,y
static float xStart = 200.0;
static float yStart = 520.0;
// health bar location offset
int32_t Hp_Xoffset = -30;
int32_t Hp_Yoffset = -50;
// Era change attribute scale
static float stats_scale = 1.5f;
static void _sdUpdate(Object* obj, uint32_t milliseconds);
static void _sdDraw(Object* obj);
// timeElapsed since last animation frame
static int32_t timeElapsed = 0;

static ObjVtable _sdVtable = {
	_sdDraw,
	_sdUpdate,
};
/*
* Init Texture of soldier
*/
void sdInitTexture(Soldier* sd, int level, int type)
{
    char file[20] = "asset/Level";
    char textureName[40];
    // assign global uv and global soldier name
    sprintf(textureName, "%s%d/%s", file, level + 1, soldierType[level][type]->name);
    if (sd->_sdTexture == 0)
    {
        sd->_sdTexture = SOIL_load_OGL_texture(textureName, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
              SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
        assert(sd->_sdTexture != 0);
    }
}
/*
* Initilize new Soldier
*/
Soldier* sdNew(int level, int type, float velX, bool reverse, base* enemyBase) {
    Soldier* sd = malloc(sizeof(Soldier));
    sd->frameCounter = 0;
    // init texture
    sd->type = type;
    sd->_sdTexture = 0;
    sdInitTexture(sd, level, type);
    // init animation frames
    sd->atFrame = 0;
    sd->wkFrame = 0;
    sd->dFrame = 0;
    sd->iFrame = 0;
    sd->reverse = reverse;
    sd->level = level;
    // assign uv and soldier name
    sd->sdUV = malloc(2 * (sizeof(int32_t)));
    sd->sdName = malloc(20 * (sizeof(char)));
    sd->animState = 0;
    sd->sdUV[0] = soldierType[level][type]->uv[0];
    sd->sdUV[1] = soldierType[level][type]->uv[1];
    strncpy(sd->sdName, soldierType[level][type]->name, 20);
    sd->sdName[19] = '\0';
    // init projectile and basic attack range
    sd->projectile_type = -1;
    sd->atkRange = 70;
    // localize velocity for speed tweaking
    sd->speed = velX;
    // ranged unit tweak
    if (type == 1) {
        sd->Hp = 50;
        sd->initHp = 50;
        sd->atkRange = 200;
        sd->atkPower = 50;
        sd->projectile_type = 0;
    } 
    // player basic melee unit
    if (type == 0) {
        sd->Hp = 110;
        sd->initHp = 110;
        sd->atkPower = 50;
    }
    // player strong melee unit
    if (type == 2) {
        sd->Hp = 250;
        sd->initHp = 250;
        sd->atkPower = 60;
        // tweak knights speed
        if (level == 1) {
            sd->speed = velX + speedOffset;
        }
    }
    // AI basic melee unit
    if (type == 3) {
        sd->Hp = 100;
        sd->initHp = 100;
        sd->atkPower = 40;
    }
    // AI special unit
    if (type == 4) {
        sd->Hp = 170;
        sd->initHp = 170;
        sd->atkPower = 70;
        // special unit speed tweak
        if (sd->level == 0 || sd->level == 1) {
            sd->speed -= speedOffset;
        }
    }
    // AI strong melee unit
    if (type == 5) {
        sd->Hp = 280;
        sd->initHp = 280;
        sd->atkPower = 80;
    }
    // skeleton archer tweak
    if (sd->level == 2 && type == 4) {
        sd->Hp = 50;
        sd->initHp = 50;
        sd->atkRange = 200;
        sd->atkPower = 40;
    }
    //  wizard tweak
    if (sd->type == 1 && sd->level == 1) {
        sd->atkRange = 250;
        sd->atkPower = 80;
        sd->Hp = 70;
        sd->initHp = 70;
        sd->projectile_type = 1;
    }
    // priest tweak
    if (sd->type == 1 && sd->level == 2) {
        sd->atkPower = 30;
        sd->Hp = 100;
        sd->initHp = 100;
        sd->projectile_type = 3;
    }
    // change projectile type for AI ranged unit level 2 (skele archer)
    if (sd->type == 4 && sd->level == 2) {
        sd->projectile_type = 2;
    }

    // Era change, attribute scaling
    if (level >= 1) {
        sd->Hp *= stats_scale;
        sd->initHp *= stats_scale;
        sd->atkPower = (int32_t)(sd->atkPower * stats_scale);
    }
    if (level == 2) {
        sd->Hp *= stats_scale;
        sd->initHp *= stats_scale;
        sd->atkPower = (int32_t)(sd->atkPower * stats_scale);
    }

    // Set up collidable
    sd->collidable_index = add_collidable(sd);

    // init health bar
    float uibar_height = yStart + Hp_Yoffset;
    if (sd->type == 2 && sd->level == 1) {
        uibar_height -= 30;
    }
    // tweak health bar location for some unit
    if (sd->type == 4 && sd->level == 1) {
        uibar_height -= 30;
    }
    // init health bar
    sd->healthBar = init_UIbar(xStart + Hp_Xoffset, uibar_height, 255, 0, 0, 0, 255, 0, 50, 5, true);
    // init start location
    Coord2D pos = { xStart, yStart };
    if (reverse) {
        pos.x = 1500 - xStart;
    }
    // init velocity
    Coord2D vel = { sd->speed,0 };
    if (sd != NULL) {
        objInit(&sd->obj, &_sdVtable, pos, vel);
    }
    // assign enemy base
    sd->enemyBase = enemyBase;

    return sd;
}
/*
* Soldier dies
*/
static void sd_die(Soldier* sd) {
    sd->animState = 2;
    UIBarDelete(sd->healthBar);
    // if player soldier die, reduce player army strength
    if (!sd->reverse) {
        decPlayerArmyStr(sd->type);
    }
    // AI sd die, reduce army strength
    else {
        decAIArmyStr(sd->type);
        // update player gold
        updateGold((int32_t)(soldier_price[sd->type-3] * (sd->level + 1) * enemyPrice_offset));
    }
}

/*
* Soldier receive damage
*/
void receive_dmg(Soldier* sd, int32_t dmg) {
    sd->Hp -= dmg;
    if (sd->Hp <= 0) {
        sd_die(sd);
    }
}

/*
* Soldier receive heal
*/
void receive_heal(Soldier* sd, int32_t heal) {
    if (sd->Hp + heal > sd->initHp) {
        sd->Hp = sd->initHp;
        return;
    }
    sd->Hp += heal;
}

/*
* Detect soldier collision with each other and enemy base
* Change animation state accordingly
*/
void soldierCollision(Soldier* sd) {
    Soldier** soldierArr = getCollidable();
    int32_t maxSoldier = getMax();
    int32_t minDist = 1000;
    int32_t minAtk = 1000;
    Soldier* target = NULL;
    Soldier* atkTarget = NULL;
    Soldier* healTarget = NULL;
    for (int i = 0; i < maxSoldier; i++) {
        Soldier* temp = soldierArr[i];
        // check null
        if (temp == NULL) {
            continue;
        }
        // check is self 
        if (sd->collidable_index == temp->collidable_index) {
            continue;
        }
        // if dying, continue
        if (temp->animState == 2) {
            continue;
        }
        // if checked target not in front, continue
        if (sd->reverse) {
            if (sd->obj.position.x < temp->obj.position.x) {
                continue;
            }
        }
        else {
            if (sd->obj.position.x > temp->obj.position.x) {
                continue;
            }
        }
        // [Game break bug fix] Change soldier position when overlapped
        if (sd->obj.position.x == temp->obj.position.x) {
            sd->obj.position.x += 1;
        }
        // check if sd is in range of i
        int32_t temp_dist = abs((int)temp->obj.position.x - (int)sd->obj.position.x);
        if (temp_dist < minDist){
            minDist = temp_dist;
            
            // heal target is minimum distance friendly
            target = temp;
            if (minDist < sd->atkRange && temp->reverse == sd->reverse) {
                healTarget = temp;
            }
        }
        // check if there is available attack target
        if (temp->reverse != sd->reverse) {
            if (temp_dist < minAtk) {
                minAtk = temp_dist;
                if (minAtk <= sd->atkRange) {
                    // attack target is minimum distance enemy
                    atkTarget = temp;
                }
            }
        }
    }

    // Have attack target, attack
    if (atkTarget != NULL) {
        // Assign target, animation state, stop movement
        sd->atkTarget = &atkTarget->obj;
        sd->animState = 1;
        objSetVel(&sd->obj, 0.0f);
        // At frame 4, deal damage or apply heal
        if (sd->atFrame == 4) {
            sd->atFrame += 1;
            receive_dmg(atkTarget, sd->atkPower);
            // priest heal
            if (healTarget != NULL && sd->type == 1 && sd->level == 2) {
                receive_heal(healTarget, 30);
            }
        }
        // if is ranged unit, init projectile
        if (sd->atFrame == 3 && (sd->type == 1 || (sd->level == 2 && sd->type == 4))) {
            sd->atFrame += 1;
            assert(sd->atkTarget != NULL);
            assert(sd->projectile_type != -1);
            Coord2D projStart = { sd->obj.position.x + 50, sd->obj.position.y };
            projectile_init(projStart, sd->atkTarget->position, sd->reverse, sd->level, sd->projectile_type);
        }
    }
    // not in range for attack, move
    else if (minDist > 70) {
        if (sd->reverse) {
            objSetVel(&sd->obj, sd->speed);
        }
        else {
            objSetVel(&sd->obj, sd->speed);
        }
        sd->animState = 0;
    }
    // friendly in front, stop
    else {
        sd->animState = 3;
        objSetVel(&sd->obj, 0.0f);
    }
    // if no attack target and no friendly in front, check if in range of enemyBase
    if (atkTarget == NULL && minDist > 70) {
        // check if in range of enemy base
        float baseX = sd->enemyBase->base_collide_X;
        // in range, attack
        if (abs((int32_t)(sd->obj.position.x - baseX)) < sd->atkRange) {
            sd->atkTarget = &sd->enemyBase->obj;
            sd->animState = 1;
            objSetVel(&sd->obj, 0.0f);
            if (sd->atFrame == 4) {
                sd->atFrame += 1;
                takeDamage(sd->enemyBase, sd->atkPower);
            }
            // if is ranged unit, init projectile
            if (sd->atFrame == 4 && (sd->type == 1 || (sd->level == 2 && sd->type == 4))) {
                sd->atFrame += 1;
                assert(sd->atkTarget != NULL);
                assert(sd->projectile_type != -1);
                Coord2D projStart = { sd->obj.position.x + 50, sd->obj.position.y };
                projectile_init(projStart, (Coord2D){ baseX, sd->obj.position.y }, sd->reverse, sd->level, sd->projectile_type);
            }
        }
        return;
    }
    else {
        // reset attack frame when attack target is null and not near base
        if (atkTarget == NULL) {
            sd->atFrame = 0;
        }
    }
    target = NULL;
    atkTarget = NULL;
}

void updateHealthBar(Soldier* sd) {
    float uibar_height = yStart + Hp_Yoffset;
    // tweak health bar location for some unit
    if (sd->type == 2 && sd->level == 1) {
        uibar_height -= 30;
    }
    if (sd->type == 4 && sd->level == 1) {
        uibar_height -= 30;
    }
    sd->healthBar->x = sd->obj.position.x + Hp_Xoffset;
    sd->healthBar->y = uibar_height;
    sd->healthBar->percent = sd->Hp / sd->initHp;
}
/*
* Soldier update
*/
static void _sdUpdate(Object* obj, uint32_t milliseconds)
{
    Soldier* sd = (Soldier*)obj;
    // count milliseconds
    sd->frameCounter += milliseconds;
    // if not dead, check collision
    if (sd->animState != 2) {
        soldierCollision(sd);
    }
    // update healthbar and location
    updateHealthBar(sd);
    objDefaultUpdate(obj, milliseconds);
}
/*
* Draw soldier
*/
static void _sdDraw(Object* obj) {
    Soldier* sd = (Soldier*)obj;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sd->_sdTexture);
    // Make visual pixel like
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBegin(GL_TRIANGLE_STRIP);
    
    {
        GLfloat xPositionLeft = obj->position.x - 150;
        GLfloat xPositionRight = xPositionLeft + 300;
        GLfloat yPositionTop = obj-> position.y - 150;
        GLfloat yPositionBottom = yPositionTop + 300;

        float u = 1.0f / sd->sdUV[0];
        float v = 1.0f / sd->sdUV[1];

        int32_t row;
        int32_t frame;
        // walk state
        if (sd->animState == 0) {
            row = 2;
            frame = sd->wkFrame;
            // advance frame every 150 ms
            if (sd->frameCounter >= 150) {
                sd->wkFrame += 1;
                sd->frameCounter = 0;
            }
            if (sd->wkFrame > 5) {
                sd->wkFrame = 0;
            }
        }
        // attack state
        else if (sd->animState == 1) {
            row = soldierType[sd->level][sd->type]->at[0];
            frame = sd->atFrame;
            int nextFrame = 150;
            //wizard attack tweak
            if (sd->level == 1 && sd->type == 1) {
                nextFrame = 400;
            }
            // advance frame at customized time
            if (sd->frameCounter >= nextFrame) {
                sd->atFrame += 1;
                sd->frameCounter = 0;
            }
            // reach last frame, reset
            if (sd->atFrame > soldierType[sd->level][sd->type]->at[1]-1) {
                sd->atFrame = 0;
            }
        }
        // death state
        else if (sd->animState == 2){
            row = sd->sdUV[1];
            frame = sd->dFrame;
            if (sd->frameCounter >= 150) {
                sd->dFrame += 1;
                sd->frameCounter = 0;
            }
            // Finish playing death animation, despawn
            if (sd->dFrame > 4) {
                // deinit object
                objDeinit(obj);
                remove_collidable(sd->collidable_index);
            }
        }
        // idle state, logic same as walking
        else if (sd->animState == 3) {
            row = 1;
            frame = sd->iFrame;
            if (sd->frameCounter >= 150) {
                sd->iFrame += 1;
                sd->frameCounter = 0;
            }
            if (sd->iFrame > 5) {
                sd->iFrame = 0;
            }
        }

        GLfloat xTextureCoord = (frame) * u;
        GLfloat yTextureCoord = (sd->sdUV[1] - row + 1) * v;
        const float BG_DEPTH = -0.99f;
    
        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
        // TL
        if (sd->reverse) {
            glTexCoord2f(xTextureCoord + u, yTextureCoord);
        }
        else {
            glTexCoord2f(xTextureCoord, yTextureCoord);
        }
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        // BL
        if (sd->reverse) {
            glTexCoord2f(xTextureCoord + u, yTextureCoord-v);
        }
        else {
            glTexCoord2f(xTextureCoord, yTextureCoord-v);
        }
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        // TR
        if (sd->reverse) {
        glTexCoord2f(xTextureCoord, yTextureCoord);
         }
        else {
        glTexCoord2f(xTextureCoord + u, yTextureCoord);
        }
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        // BR
        if (sd->reverse) {
            glTexCoord2f(xTextureCoord, yTextureCoord - v);
        }
        else {
            glTexCoord2f(xTextureCoord + u, yTextureCoord - v);
        }
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
    }
    glEnd();
}