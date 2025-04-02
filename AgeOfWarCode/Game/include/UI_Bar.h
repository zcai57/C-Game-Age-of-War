#pragma once
#include "baseTypes.h"
#include "Object.h"
#ifdef __cplusplus
#endif	
typedef struct UIBar_t {
	Object obj;
	float x;
	float y;
	uint8_t r1;
	uint8_t g1;
	uint8_t b1;
	uint8_t r2;
	uint8_t b2;
	uint8_t g2;
	bool rowStyle;
	float percent;
	float length;
	float height;
}UIBar;
UIBar* init_UIbar(float x, float y, uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2,
	float length, float height, bool rowStyle);
void UIBarDelete(UIBar* bar);
#ifdef __cplusplus

#endif

