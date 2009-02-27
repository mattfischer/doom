#include <math.h>
#include "global.h"
#include "game.h"

#include "Level.h"
#include "Debug.h"
#include "DDraw.h"
#include "Draw.h"
#include "Input.h"
#include "Edit.h"

unsigned long frametimer;

//#define FunctionLog
GameState::GameState(GraphicsContext *context)
{
	mContext = context;

	#ifdef FunctionLog
	DebugString("LoadLevel()\n");
	#endif
	mLevel = LoadLevel();
		
	mMapInfo.rotate = 0;
	mMapInfo.zoom = 10;
	mMapInfo.show = 0;

	#ifdef FunctionLog
	DebugString("SetupDirectDraw()\n");
	#endif

	InitWalls();

	frametimer = GetTickCount();
}

GameState::~GameState()
{
}

void GameState::runIteration()
{
	mContext->setLocked(true);

	ProcessInput(mLevel->player, &mMapInfo);

	DrawScreen(mContext, mLevel);
	if(mMapInfo.show) DrawOverhead(mContext, &mMapInfo, mLevel);

	mContext->setLocked(false);
	mContext->flip();
}	

void GameState::mouseMoved(int x, int y, bool buttonDown)
{
	if(buttonDown)
		MovePoints(mLevel->player, &mMapInfo, x, y);
}

void GameState::mouseButtonDown(int x, int y)
{
	SelectPoint(mLevel, &mMapInfo, x, y);
}
