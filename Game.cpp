#include <math.h>
#include "global.h"
#include "game.h"

int frames=0;
int flip=1;
//#define FunctionLog
int Game_Init()
{
	#ifdef FunctionLog
	DebugString("LoadLevel()\n");
	#endif
	LoadLevel();
		
	#ifdef FunctionLog
	DebugString("SetupDirectDraw()\n");
	#endif

#ifndef NOVIS
	SetupDirectDraw();
#endif

	InitWalls();

	DebugString("Made it through Game_Init\n");
	timer=GetTickCount();
	frametimer=GetTickCount();
	return 0;
}

void Game_Main()
{
	char buffer[100];
#ifndef NOVIS
	LockBack();
#endif
	#ifdef FunctionLog
	DebugString("ProcessInput()\n");
	#endif
	ProcessInput();
	#ifdef FunctionLog
	DebugString("DrawScreen()\n");
	#endif
	DrawScreen();
#ifndef NOVIS
	UnlockBack();
#endif
	#ifdef FunctionLog
	DebugString("FlipSurfaces()\n");
	#endif
#ifndef NOVIS
	FlipSurfaces();
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

void Game_Shutdown()
{
	
	DisplayMode(0);
	CleanupDirectDraw();
	CloseHandle(DebugFile);
}
