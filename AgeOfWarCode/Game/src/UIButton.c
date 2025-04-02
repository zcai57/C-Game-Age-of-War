#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "baseTypes.h"
#include "Projectile.h"
#include "Object.h"
#include <gl/GLU.h>
#include "SOIL.h"
/// Very simple texture render class

typedef struct button_T {
	Object obj;
    GLuint _btTexture;
    int32_t width;
    int32_t height;
} button;

static void _btUpdate(Object* obj, uint32_t milliseconds);
static void _btDraw(Object* obj);

static ObjVtable _btVtable = {
	_btDraw,
	_btUpdate,
};
/*
* Reset texture
*/
void resetButtonTexture(button* bt) {
    bt->_btTexture = 0;
}

void buttonInitTexture(button* bt, char* name) {
    if (bt->_btTexture == 0) {
        bt->_btTexture = SOIL_load_OGL_texture(name, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
        assert(bt->_btTexture != 0);
    }
}
/*
* Init button
*/
button* button_init(Coord2D pos, char* name, int32_t width, int32_t height) {
	button* bt = malloc(sizeof(button));
	Coord2D vel = { 0.0f, 0.0f };
    bt->_btTexture = 0;
    bt->width = width;
    bt->height = height;
    buttonInitTexture(bt, name);
	objInit(&bt->obj, &_btVtable, pos, vel);
    return bt;
};

void button_deinit(Object* obj) {
	objDeinit(obj);
	free(obj);
}

void _btUpdate(Object* obj, uint32_t milliseconds) {
	objDefaultUpdate(obj, milliseconds);
}

/*
* Button draw
* Default to Draw whole texture
*/
void _btDraw(Object* obj) {
    button* bt = (button*)obj;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bt->_btTexture);
    glBegin(GL_TRIANGLE_STRIP);

    GLfloat xPositionLeft = obj->position.x;
    GLfloat xPositionRight = xPositionLeft + (float)bt->width;
    GLfloat yPositionTop = obj->position.y;
    GLfloat yPositionBottom = yPositionTop + (float)bt->height;

    float u = 1.0f / bt->width;
    float v = 1.0f / bt->height;

    GLfloat xTextureCoord = 0.0f;
    GLfloat yTextureCoord = 1.0f;
    const float BG_DEPTH = -0.99f;
    {
        glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
        // TL
        glTexCoord2f(xTextureCoord, yTextureCoord);
        glVertex3f(xPositionLeft, yPositionTop, BG_DEPTH);

        // BL
        glTexCoord2f(xTextureCoord, 0);
        glVertex3f(xPositionLeft, yPositionBottom, BG_DEPTH);

        // TR
        glTexCoord2f(1, yTextureCoord);
        glVertex3f(xPositionRight, yPositionTop, BG_DEPTH);

        // BR
        glTexCoord2f(1, 0);
        glVertex3f(xPositionRight, yPositionBottom, BG_DEPTH);
    }
    glEnd();
}