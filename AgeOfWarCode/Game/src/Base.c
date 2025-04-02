#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "Object.h"
#include "SOIL.h"
#include "Base.h"
#include "input.h"
#include "UI_Bar.h"

// texture name table for bases
static char baseName[3][30] = { "asset/Base1.png", "asset/Base2.png", "asset/Base3.png" };
static void _bsUpdate(Object* obj, uint32_t milliseconds);
static void _bsDraw(Object* obj);

static ObjVtable _bsVtable = {
    _bsDraw,
    _bsUpdate,
};
/*
* Base init texture
*/
void baseInitTexture(base* bs, int level)
{
    if (bs->_baseTexture == 0)
    {
        bs->_baseTexture = SOIL_load_OGL_texture(baseName[level-1], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
        assert(bs->_baseTexture != 0);
    }
}
/*
* init base
*/
base* base_init(Coord2D pos, float hp, int32_t level, bool reverse) {
	// init base object and texture
    base* bs = malloc(sizeof(base));
	assert(bs != NULL);
	bs->_baseTexture = 0;
	bs->hp = hp;
	bs->maxHp = hp;
    baseInitTexture(bs, level);
	bs->reverse = reverse;

	if (reverse) {
		bs->base_collide_X = pos.x;
	}
	else {
		bs->base_collide_X = pos.x + 300.0f;
	}
	// init object
    objInit(&bs->obj, &_bsVtable, pos, (Coord2D) {0.0f, 0.0f});
	bs->hpUI = init_UIbar(pos.x + 180, pos.y - 200, 255,255,255, 255, 0, 0, 20, 200, false);
	return bs;
}
/*
* base take damge
*/ 
void takeDamage(base* bs, int dmg) {
	if (bs == NULL) {
		return;
	}
	bs->hp -= dmg;
	bs->hpUI->percent = bs->hp / bs->maxHp;
}
/*
* Getter function for base collision point
*/
float getCollideX(base* bs) {
	return bs->base_collide_X;
}
/*
* Deinit base
*/
void base_deinit(base* bs) {
	objDeinit(&bs->obj);
	assert(bs != NULL);
	free(bs);
}

void _bsUpdate(Object* obj, uint32_t milliseconds) {
    objDefaultUpdate(obj, milliseconds);
}
/*
* Draw base
*/
void _bsDraw(Object* obj) {
	base* bs = (base*)obj;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bs->_baseTexture);
	glBegin(GL_TRIANGLE_STRIP);

	GLfloat xPositionLeft = obj->position.x;
	GLfloat xPositionRight = obj->position.x + 387;
	GLfloat yPositionTop = obj->position.y;
	GLfloat yPositionBottom = obj->position.y + 287;

	float u = 1.0f / 232;
	float v = 1.0f / 169;

	GLfloat xTextureCoord = 0.0f;
	GLfloat yTextureCoord = 1.0f;
	const float BG_DEPTH = -0.99f;
	{
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
		// TL
		if (bs->reverse) {
			glTexCoord2f(1, yTextureCoord);
		}
		else {
			glTexCoord2f(xTextureCoord, yTextureCoord);
		}
		glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

		// BL
		if (bs->reverse) {
			glTexCoord2f(1, 0);
		}
		else {
			glTexCoord2f(xTextureCoord, 0);
		}
		glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

		// TR
		if (bs->reverse) {
			glTexCoord2f(xTextureCoord, yTextureCoord);
		}
		else {
			glTexCoord2f(1, yTextureCoord);
		}
		glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

		// BR
		if (bs->reverse) {
			glTexCoord2f(xTextureCoord, 0);
		}
		else {
			glTexCoord2f(1, 0);
		}
		glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
	}
	glEnd();
}

