#pragma once
#include "Object.h"
#ifdef __cplusplus
#endif	
typedef struct button_t button;
	button* button_init(Coord2D pos, char* name, int32_t width, int32_t height);
	void button_deinit(Object* obj);
	void resetButtonTexture(button* bt);
	void buttonInitTexture(button* bt, char* name);
#ifdef __cplusplus

#endif
