#include <math.h>
#include "global.h"
#include "game.h"

#include "Level.h"
#include "Debug.h"
#include "DDraw.h"
#include "Draw.h"
#include "Input.h"

int frames=0;
int flip=1;
Level *level;
unsigned long frametimer;
DWORD dummy;
unsigned long timer;

//#define FunctionLog
int Game_Init()
{
	#ifdef FunctionLog
	DebugString("LoadLevel()\n");
	#endif
	level = LoadLevel();
		
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
	char buffer[100];
#ifndef NOVIS
	LockBack(context);
#endif
	#ifdef FunctionLog
	DebugString("ProcessInput()\n");
	#endif
	ProcessInput(level->player);
	#ifdef FunctionLog
	DebugString("DrawScreen()\n");
	#endif
	DrawScreen(level, context);
#ifndef NOVIS
	UnlockBack(context);
#endif
	#ifdef FunctionLog
	DebugString("FlipSurfaces()\n");
	#endif
#ifndef NOVIS
	FlipSurfaces(context);
#endif
	
	frames++;
		if(GetTickCount()>timer+(unsigned)1000)
			{
				timer=GetTickCount();
				wsprintf(buffer,"%i fps\n",frames);
				DebugString(buffer);
				frames=0;
				
			}

}

void Game_Shutdown(GraphicsContext *context)
{
	DisplayMode(context, 0);
	CleanupDirectDraw(context);
	DebugShutdown();
}
