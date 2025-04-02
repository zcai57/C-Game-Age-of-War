#pragma once
#include "Object.h"
#ifdef __cplusplus
#endif	
typedef struct numdisplay_t display; 
display* numRender_init(int32_t number, Coord2D position, int8_t digit);
void numRender_update(display* dp, int32_t number);
#ifdef __cplusplus

#endif