#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <stdlib.h>
#include <ddraw.h>

#include "World.h"

class Level;
class Renderer;
class GraphicsContext;

struct MapInfo {
	int show;
	int rotate;
	double zoom;
};

class GameState {
public:
	GameState(GraphicsContext *context);
	~GameState();

	void runIteration();

	void mouseMoved(int x, int y, bool buttonDown);
	void mouseButtonDown(int x, int y);

protected:
	GraphicsContext *mContext;
	Level *mLevel;
	Player mPlayer;
	MapInfo mMapInfo;
	Renderer *mRenderer;
};

#endif
