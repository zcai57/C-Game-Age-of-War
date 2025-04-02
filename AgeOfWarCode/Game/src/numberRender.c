#include "UIButton.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/*
* Texture table for numbers
*/
static char numberAsset[10][20] = {
	"asset/0.png",
	"asset/1.png",
	"asset/2.png",
	"asset/3.png",
	"asset/4.png",
	"asset/5.png",
	"asset/6.png",
	"asset/7.png",
	"asset/8.png",
	"asset/9.png",
};
// texture x position offset
float positionX_offset = 27;

typedef struct numdisplay_t {
	// array of ui_button for each displayed number
	button** displayButton;
	// number of displayed number
	int8_t digit;
}display;


/*
* Display customized amount of numbers
*/ 
display* numRender_init(int32_t number, Coord2D position, int8_t digit) {
	// curNumber to display
	int32_t curNumber = number;
	// remaining number to be calcualted
	int32_t remainNumber = number;
	display* dp = malloc(sizeof(display));
	dp->digit = digit;
	dp->displayButton = malloc(digit * sizeof(button*));
	for (int i = digit - 1; i >= 0; i--) {
		// curNumber is quotient
		curNumber = remainNumber / (int32_t)(pow(10, i));
		// remaining number is remainder
		remainNumber = remainNumber % (int32_t)(pow(10, i));
		assert(curNumber < 10);
		Coord2D newPos = { position.x + (digit - i) * positionX_offset, position.y };
		// init display
		dp->displayButton[i] = button_init(newPos, numberAsset[curNumber], 27, 39);
	}
	return dp;
}

/*
* Update number display
*/
void numRender_update(display* dp, int32_t number) {
	int32_t curNumber = number;
	int32_t remainNumber = number;
	for (int i = dp->digit - 1; i >= 0; i--) {
		curNumber = remainNumber / (int32_t)(pow(10, i));
		remainNumber = remainNumber % (int32_t)(pow(10, i));
		assert(curNumber < 10);
		// reset texture and apply new texture according to calculated number
		resetButtonTexture(dp->displayButton[i]);
		buttonInitTexture(dp->displayButton[i], numberAsset[curNumber]);
	}
}