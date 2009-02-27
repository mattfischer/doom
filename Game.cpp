#include <math.h>
#include "global.h"
#include "game.h"

#include "Level.h"
#include "Debug.h"
#include "GraphicsContext.h"
#include "Renderer.h"
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

	mRenderer = new Renderer(mContext, mLevel);

	mRenderer->setHorizon(mContext->height() / 2);

	frametimer = GetTickCount();
}

GameState::~GameState()
{
	delete mRenderer;
}

void GameState::runIteration()
{
	mContext->setLocked(true);

	int horizon = mRenderer->horizon();
	ProcessInput(mLevel->player, &mMapInfo, &horizon);

	mRenderer->setHorizon(horizon);

	mRenderer->drawScreen();
	if(mMapInfo.show) mRenderer->drawMap(&mMapInfo);

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
