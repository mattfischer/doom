#include <math.h>
#include "Game.h"

#include "Level.h"
#include "Debug.h"
#include "GraphicsContext.h"
#include "Renderer.h"
#include "Edit.h"
#include "Physics.h"

//#define FunctionLog
GameState::GameState(GraphicsContext *context)
{
	mContext = context;

	#ifdef FunctionLog
	DebugString("LoadLevel()\n");
	#endif
	mLevel = LoadLevel();
		
	mMapRotate = 0;
	mMapZoom = 10;
	mShowMap = false;

	mPlayer = mLevel->playerStart;

	#ifdef FunctionLog
	DebugString("SetupDirectDraw()\n");
	#endif

	mRenderer = new Renderer(mContext, mLevel);
	mRenderer->setHorizon(mContext->height() / 2);

	mEditor = new Editor(mLevel);

	mFrameTimer = GetTickCount();
}

GameState::~GameState()
{
	delete mRenderer;
	delete mEditor;
}

void GameState::runIteration()
{
	mContext->setLocked(true);

	processInput();

	mRenderer->drawScreen(&mPlayer);
	if(mShowMap) mRenderer->drawMap(&mPlayer, mMapRotate, mMapZoom, mEditor->selectedPoint());

	mContext->setLocked(false);
	mContext->flip();
}	

void GameState::mouseMoved(int x, int y, bool buttonDown)
{
	if(buttonDown && mShowMap)
		mEditor->movePoints(&mPlayer, x, y, mMapRotate, mMapZoom);
}

void GameState::mouseButtonDown(int x, int y)
{
	if(mShowMap) 
		mEditor->selectPoint(&mPlayer, x, y, mMapRotate, mMapZoom);
}

int tabflag=0;
int plusflag=0;
int minusflag=0;
int playbyplay=0;

#define PI 3.14159
#define KEY_DOWN(i) ((GetAsyncKeyState(i) & 0x8000) ? 1 : 0)
#define KEY_UP(i) ((GetAsyncKeyState(i) & 0x8000) ? 0 : 1)

void GameState::processInput()
{
	double stepsize = 25;
	double anglesize = 2;
	double newx;
	double newy;
	
	int horizon = mRenderer->horizon();

	int frameTime = GetTickCount() - mFrameTimer;
	mFrameTimer = GetTickCount();

	if(KEY_DOWN(VK_F1)) playbyplay = 1;
	if(KEY_DOWN(VK_TAB))
	{
		if(tabflag == 0)
		{
			mShowMap = !mShowMap;
			tabflag = 1;
		}
	}

	if(KEY_DOWN(VK_ADD)) 
	{
		if(plusflag == 0)
		{
			mMapZoom *= 1.5;
			plusflag = 1;
		}
	}
	if(KEY_DOWN(VK_SUBTRACT)) 
	{
		if(minusflag == 0)
		{
			mMapZoom *= .66;
			minusflag = 1;
		}
	}

	if(KEY_DOWN(VK_F11)) mMapRotate = true;
	if(KEY_DOWN(VK_F12)) mMapRotate = false;

	if(KEY_UP(VK_ADD)) plusflag = 0;
	if(KEY_UP(VK_SUBTRACT)) minusflag = 0;

	if(KEY_UP(VK_TAB)) tabflag = 0;
	if(KEY_DOWN(VK_ESCAPE)) PostQuitMessage(0);
	if(KEY_DOWN(VK_PRIOR)) horizon += 20;
	if(KEY_DOWN(VK_NEXT)) horizon -= 20;
	if(KEY_DOWN('A')) mPlayer.height += .1;
	if(KEY_DOWN('Z')) mPlayer.height -= .1;
	if(KEY_DOWN(VK_RIGHT))
	{
		mPlayer.angle -= anglesize * frameTime / 1000;
		if(mPlayer.angle < 0) mPlayer.angle += 2 * PI;
	}
	if(KEY_DOWN(VK_LEFT))
	{
		mPlayer.angle += anglesize * frameTime / 1000;
		if(mPlayer.angle > 2 * PI) mPlayer.angle -= 2 * PI;
	}
	if(KEY_DOWN(VK_UP))
	{
		newx = mPlayer.x + cos(mPlayer.angle) * stepsize * frameTime / 1000;
		newy = mPlayer.y + sin(mPlayer.angle) * stepsize * frameTime / 1000;
		DoWallCollisions(&mPlayer, newx, newy);
	}
	if(KEY_DOWN(VK_DOWN))
	{
		newx = mPlayer.x - cos(mPlayer.angle) * stepsize * frameTime / 1000;
		newy = mPlayer.y - sin(mPlayer.angle) * stepsize * frameTime / 1000;
		DoWallCollisions(&mPlayer, newx, newy);
	}

	mRenderer->setHorizon(horizon);
}

