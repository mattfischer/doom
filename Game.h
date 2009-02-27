#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <stdlib.h>
#include <ddraw.h>

#include "World.h"

class Level;
class Renderer;
class GraphicsContext;
class Editor;

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
	void processInput();

	GraphicsContext *mContext;
	Level *mLevel;
	Player mPlayer;
	Renderer *mRenderer;
	Editor *mEditor;

	unsigned long mFrameTimer;
	bool mShowMap;
	int mMapRotate;
	double mMapZoom;
};

#endif
