#pragma
#include "UI_Bar.h"
#include "baseTypes.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "Object.h"
#include "SOIL.h"
#include "input.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct base_T {
	Object obj;
	GLuint _baseTexture;
	int32_t collidable_index;
	bool reverse;
	UIBar* hpUI;
	float hp;
	float maxHp;
	float base_collide_X;
} base;
	base* base_init(Coord2D pos, float hp, int32_t level, bool reverse);
	void base_deinit(base* bs);
	void takeDamage(base* bs, int dmg);
	void baseInitTexture(base* bs, int level);
	float getCollideX(base* bs);
#ifdef __cplusplus
}
#endif

