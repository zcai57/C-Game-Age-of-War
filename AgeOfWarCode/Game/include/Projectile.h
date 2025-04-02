#pragma once
#include "Object.h"
#ifdef __cplusplus
#endif	
void projectile_init(Coord2D start, Coord2D target, boolean reverse, int level, int projectile_type);
void projectile_deinit(Object* obj);
#ifdef __cplusplus

#endif
