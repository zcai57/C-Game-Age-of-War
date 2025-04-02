#pragma once
#include "levelmgr.h"
#ifdef __cplusplus
extern "C" {
#endif
	void AI_New();
	void AI_Update(Level* level, uint32_t milliseconds);
	void decAIArmyStr(int type);
#ifdef __cplusplus
}
#endif
