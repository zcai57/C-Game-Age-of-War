#include "baseTypes.h"
#include "input.h"
#include "application.h"
#include "framework.h"
#include "levelmgr.h"
#include "objmgr.h"
#include "AIcontroller.h"
#include "shape.h"

static uint32_t timeElapsed = 0;
static void _gameInit();
static void _gameShutdown();
static void _gameDraw();
static void _gameUpdate(uint32_t milliseconds);

static bool gameOver = false;


static LevelDef _levelDefs[] = {
	{
		{{50, 50}, {974, 600}},		// fieldBounds
		0x00ff0000,					// fieldColor
		20,							// numBalls
		10							// numFaces
	}
};
static Level* _curLevel = NULL;

static int mouseClickCount = 0;
/// @brief Program Entry Point (WinMain)
/// @param hInstance 
/// @param hPrevInstance 
/// @param lpCmdLine 
/// @param nCmdShow 
/// @return 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const char GAME_NAME[] = "Framework1";

	Application* app = appNew(hInstance, GAME_NAME, _gameDraw, _gameUpdate);

	if (app != NULL)
	{
		GLWindow* window = fwInitWindow(app);
		if (window != NULL)
		{
			_gameInit();

			bool running = true;
			while (running)
			{
				running = fwUpdateWindow(window);
			}

			_gameShutdown();
			fwShutdownWindow(window);
		}

		appDelete(app);
	}
}

/// @brief Initialize code to run at application startup
static void _gameInit()
{
	const uint32_t MAX_OBJECTS = 500;
	objMgrInit(MAX_OBJECTS);
	levelMgrInit();

	_curLevel = levelMgrLoad(&_levelDefs[0]);
}

/// @brief Cleanup the game and free up any allocated resources
static void _gameShutdown()
{
	levelMgrUnload(_curLevel);

	levelMgrShutdown();
	objMgrShutdown();
}

/// @brief Draw everything to the screen for current frame
static void _gameDraw() 
{
	objMgrDraw();
}
/*
* Update experience point
*/
static void updateExp(uint32_t milliseconds) {
	timeElapsed += milliseconds;
	if (timeElapsed >= 1000) {
		timeElapsed = 0;
		incLevelExp();
		updateLevelExpUI(_curLevel);
	}
}

/// @brief Perform updates for all game objects, for the elapsed duration
/// @param milliseconds 
static void _gameUpdate(uint32_t milliseconds)
{
	// ESC exits the program
	if (inputKeyPressed(VK_ESCAPE))
	{
		// TODO 
		//TerminateApplication(window);
	}
	if (!gameOver) {
		// F1 toggles fullscreen
		if (inputKeyPressed(VK_F1))
		{
			// TODO 
			//ToggleFullscreen(window);
		}
		// press 1, spawn player melee unit
		if (inputKeyPressed(0x31)) {
			spawnSoldier(_curLevel, 0, false);
			inputKeyUpdate(0x31, false);
		}
		// press 2, spawn player ranged unit
		if (inputKeyPressed(0x32)) {
			spawnSoldier(_curLevel, 1, false);
			inputKeyUpdate(0x32, false);

		}
		// press 3, spawn player strong melee unit
		if (inputKeyPressed(0x33)) {
			spawnSoldier(_curLevel, 2, false);
			inputKeyUpdate(0x33, false);
		}
		// debug press 4, spawn enemy special unit
		/*if (inputKeyPressed(0x34)) {
			spawnSoldier(_curLevel, 4, true);
			inputKeyUpdate(0x34, false);
		}*/
		// mouse left click
		if (inputMousePressed(INPUT_BUTTON_LEFT)) {
			mouseClickCount += 1;
			if (mouseClickCount == 2) {
				mouseClickCount = 0;
				handleMouseClick(_curLevel, inputMousePosition());
			}
			inputMouseUpdateButton(INPUT_BUTTON_LEFT, false);
		}
		// Game over if player or enemy base hp is below 0
		if (AIBaseHp(_curLevel) <= 0.0f || playerBaseHp(_curLevel) <= 0.0f) {
			gameOver = true;
			initGameOverScreen();
		}
		// update exp based on time
		updateExp(milliseconds);
		// update all objects
		objMgrUpdate(milliseconds);
		// update AI
		AI_Update(_curLevel, milliseconds);
	}
	else {

	}
}