#include "shape.h"
#include "baseTypes.h"
#include "stdlib.h"
#include "Object.h"

typedef struct UIBar_t {
	Object obj;
	float x;
	float y;
	// second layer color
	uint8_t r1;
	uint8_t g1;
	uint8_t b1;
	// first layer color
	uint8_t r2;
	uint8_t b2;
	uint8_t g2;
	// row or column
	bool rowStyle;
	// second layer bar length percentage. (0-1)
	float percent;
	float length;
	float height;
} UIBar;

static void _barUpdate(Object* obj, uint32_t milliseconds);
static void _barDraw(Object* obj);
static ObjVtable _ballVtable = {
	_barDraw,
	_barUpdate
};
/*
* Delete ui bar
*/
void UIBarDelete(UIBar* bar)
{
	objDeinit(&bar->obj);

	free(bar);
}
/*
* Init ui bar
*/
UIBar* init_UIbar(float x, float y, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, 
	float length, float height, bool rowStyle) {
	UIBar* bar = malloc(sizeof(UIBar));
	// init ui bar object
	bar->x = x;
	bar->y = y;
	bar->r1 = r1;
	bar->g1 = g1;
	bar->b1 = b1;
	bar->r2 = r2;
	bar->g2 = g2;
	bar->b2 = b2;
	bar->length = length;
	bar->height = height;
	bar->rowStyle = rowStyle;
	bar->percent = 1.0f;
	Coord2D pos = { bar->x, bar->y };
	objInit(&bar->obj, &_ballVtable, pos, (Coord2D){0.0f,0.0f});

	return bar;
}

/*
* UI Bar update
*/
static void _barUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}
/*
* UI Bar draw
*/
static void _barDraw(Object* obj)
{
	UIBar* bar = (UIBar*)obj;
	// draw second layer bar
	shapeDrawRectangle(bar->x, bar->y, bar->r1, bar->g1, bar->b1, bar->length, bar->height);
	// calculate second layer bar length based on percent
	float newLength = bar->length;
	float newHeight = bar->height;
	float newY = bar->y;
	if (bar->rowStyle) {
		newLength = newLength * bar->percent;
	}
	else {
		newHeight = newHeight * bar->percent;
		newY += (bar->height - newHeight);
		
	}
	// draw second layer bar
	shapeDrawRectangle(bar->x, newY, bar->r2, bar->g2, bar->b2, newLength, newHeight);
}