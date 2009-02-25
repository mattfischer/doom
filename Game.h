#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <stdlib.h>
#include <ddraw.h>

#include "ddraw.h"

int Game_Init();
void Game_Main(GraphicsContext *context);
void Game_Shutdown(GraphicsContext *context);

#endif
