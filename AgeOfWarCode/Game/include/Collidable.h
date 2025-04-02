#pragma once
#ifdef __cplusplus
#endif

	typedef struct collidable_t Collidable;

	void collidable_Init();
	int32_t add_collidable(Soldier* sd);
	Soldier** getCollidable();
	int32_t getMax();
	void remove_collidable(int32_t num);
#ifdef __cplusplus

#endif

