#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <stdlib.h>
#include <ddraw.h>

#include "ddraw.h"

#include "World.h"
#include "Level.h"

struct GameInfo {
	Level *level;
	MapInfo mapInfo;
};

int Game_Init();
void Game_Main(GraphicsContext *context);
void Game_Shutdown(GraphicsContext *context);
void Game_MouseMove(int x, int y, bool buttonDown);
void Game_MouseButtonDown(int x, int y);

#endif
