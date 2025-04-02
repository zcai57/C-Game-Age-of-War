#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "baseTypes.h"
#include "Projectile.h"
#include "Object.h"
#include <gl/GLU.h>
#include "SOIL.h"


typedef struct projectile_T{
	Object obj;
	GLuint _pjTexture;
	Coord2D target;
	boolean reverse;
	int projectile_type; // 0, 1, 2
	int level;
} Proj;
static void _projUpdate(Object* obj, uint32_t milliseconds);
static void _projDraw(Object* obj);
// Projectile speed
static Coord2D vel = { 0.7f, 0.0f };

// Texture table for different projectile
static char proj_textures[4][30] = {
	"asset/Arrow.png",
	"asset/FireBall.png",
	"asset/BoneArrow.png",
	"asset/PriestAttack.png",
};

static ObjVtable _projVtable = {
	_projDraw,
	_projUpdate
};
/*
* Projectile init texture
*/
void projInitTexture(Proj* proj, int level) {
	if (proj->_pjTexture == 0) {
		proj->_pjTexture = SOIL_load_OGL_texture(proj_textures[proj->projectile_type], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(proj->_pjTexture != 0);
	}
}
/*
* Projectile init
*/
void projectile_init(Coord2D start, Coord2D target, boolean reverse, int level, int projectile_Type) {
	// init projectile component
	Proj* proj = malloc(sizeof(Proj));
	proj->target = target;
	proj->level = level;
	proj->reverse = reverse;
	proj->_pjTexture = 0;
	proj->projectile_type = projectile_Type;
	projInitTexture(proj, level);
	Coord2D localVel = vel;
	if (reverse) {
		localVel.x = -vel.x;
	}
	objInit(&proj->obj, &_projVtable, start, localVel);
};

/*
* Despawn projectile
*/
void projectile_deinit(Object* obj) {
	objDeinit(obj);
	assert(obj != NULL);
	free(obj);
};
/*
* Projectile update
*/
void _projUpdate(Object* obj, uint32_t milliseconds) {
	Proj* proj = (Proj*)obj;
	// if reaches target, despawn
	if(proj->reverse) {
		if (obj->position.x <= proj->target.x ) {
			projectile_deinit(obj);
		}
	}
	else {
		if (obj->position.x >= proj->target.x) {
			projectile_deinit(obj);
		}
	}
	// updates position 
	objDefaultUpdate(obj, milliseconds);
}
/*
* Draw projectile
*/
static void _projDraw(Object* obj) {
	Proj* proj = (Proj*)obj;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, proj->_pjTexture);
	glBegin(GL_TRIANGLE_STRIP);

	GLfloat xPositionLeft = obj->position.x - 100;
	GLfloat xPositionRight = obj->position.x + 100;
	GLfloat yPositionTop = obj->position.y - 100;
	GLfloat yPositionBottom = obj->position.y + 100;

	float u = 1.0f / 100;
	float v = 1.0f / 100;

	GLfloat xTextureCoord = 0.0f;
	GLfloat yTextureCoord = 1.0f;
	const float BG_DEPTH = -0.99f;
	{
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
		// TL
		if (proj->reverse) {
			glTexCoord2f(1, yTextureCoord);
		}
		else {
			glTexCoord2f(xTextureCoord, yTextureCoord);
		}
		glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

		// BL
		if (proj->reverse) {
			glTexCoord2f(1, 0);
		}
		else {
			glTexCoord2f(xTextureCoord, 0);
		}
		glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

		// TR
		if (proj->reverse) {
			glTexCoord2f(xTextureCoord, yTextureCoord);
		}
		else {
			glTexCoord2f(1, yTextureCoord);
		}
		glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

		// BR
		if (proj->reverse) {
			glTexCoord2f(xTextureCoord, 0);
		}
		else {
			glTexCoord2f(1, 0);
		}
		glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
	}
	glEnd();
}