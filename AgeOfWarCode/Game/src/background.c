#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "Object.h"
#include "SOIL.h"
#include "background.h"
#include "input.h"

typedef struct background_t {
    Object obj;

    Bounds2D bounds;
} Background;

static const char CHARACTER_PAGE[] = "asset/background.png";
static GLuint _backgroundTexture = 0;
static void _bgUpdate(Object* obj, uint32_t milliseconds);
static void _bgDraw(Object* obj);

static ObjVtable _bgVtable = {
    _bgDraw,
    _bgUpdate,
};

Background* bgNew() {
    Background* bg = malloc(sizeof(Background));
    Coord2D pos = { 0,0 };
    Coord2D vel = { 0,0 };
    if (bg != NULL) {
        objInit(&bg->obj, &_bgVtable, pos, vel);
    }
    return bg;
}
void backgroundInitTexture()
{
    if (_backgroundTexture == 0)
    {
        _backgroundTexture = SOIL_load_OGL_texture(CHARACTER_PAGE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
        assert(_backgroundTexture != 0);
    }
}

static void _bgUpdate(Object* obj, uint32_t milliseconds)
{
    objDefaultUpdate(obj, milliseconds);
   
}

static void _bgDraw(Object* obj) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _backgroundTexture);
    glBegin(GL_TRIANGLE_STRIP);

    GLfloat xPositionLeft = 0.0;
    GLfloat xPositionRight = 1500.0;
    GLfloat yPositionTop = 0.0;
    GLfloat yPositionBottom = 600.0;

    float u = 1.0f / 1080;
    float v = 1.0f / 1920;

    GLfloat xTextureCoord = 0.0f;
    GLfloat yTextureCoord = 0.7f;
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