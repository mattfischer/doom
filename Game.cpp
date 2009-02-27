#include <math.h>
#include "global.h"
#include "game.h"

#include "Level.h"
#include "Debug.h"
#include "DDraw.h"
#include "Draw.h"
#include "Input.h"
#include "Edit.h"

int frames=0;
int flip=1;
unsigned long frametimer;
unsigned long timer;
GameInfo gameInfo;

//#define FunctionLog
int Game_Init()
{
	#ifdef FunctionLog
	DebugString("LoadLevel()\n");
	#endif
	gameInfo.level = LoadLevel();
		
	gameInfo.mapInfo.rotate = 0;
	gameInfo.mapInfo.zoom = 10;
	gameInfo.mapInfo.show = 0;

	#ifdef FunctionLog
	DebugString("SetupDirectDraw()\n");
	#endif

	InitWalls();

	DebugString("Made it through Game_Init\n");
	timer=GetTickCount();
	frametimer=GetTickCount();
	return 0;
}

void Game_Main(GraphicsContext *context)
{
	context->setLocked(true);

	#ifdef FunctionLog
	DebugString("ProcessInput()\n");
	#endif
	ProcessInput(gameInfo.level->player, &gameInfo.mapInfo);

	#ifdef FunctionLog
	DebugString("DrawScreen()\n");
	#endif
	DrawScreen(context, gameInfo.level);

	if(gameInfo.mapInfo.show) DrawOverhead(context, &gameInfo.mapInfo, gameInfo.level);

	context->setLocked(false);

	#ifdef FunctionLog
	DebugString("FlipSurfaces()\n");
	#endif

	context->flip();
	
	frames++;
	if(GetTickCount()>timer+(unsigned)1000)
	{
		char buffer[100];
		timer=GetTickCount();
		wsprintf(buffer,"%i fps\n",frames);
		DebugString(buffer);
		frames=0;
		
	}
}

void Game_Shutdown(GraphicsContext *context)
{
	
}

void Game_MouseMove(int x, int y, bool buttonDown)
{
	if(buttonDown)
		MovePoints(gameInfo.level->player, &gameInfo.mapInfo, x, y);
}

void Game_MouseButtonDown(int x, int y)
{
	SelectPoint(gameInfo.level, &gameInfo.mapInfo, x, y);
}
