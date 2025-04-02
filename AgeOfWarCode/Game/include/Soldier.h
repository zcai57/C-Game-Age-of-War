#pragma once
#include "baseTypes.h"
#include "Base.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct soldier_t Soldier;

typedef struct soldierTexture {
		char name[20];
		int32_t uv[2];
		int32_t at[2];
} sdTexture;
Soldier* sdNew(int level, int type, float velX, bool reverse, base* enemyBase);
//void sdInitTexture(Object* obj, int level, int type);
//void sdInitTexture(int level, int type);
#ifdef __cplusplus
}
#endif