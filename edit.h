#ifndef EDIT_H
#define EDIT_H

#include "World.h"

struct Level;
struct Player;
struct MapInfo;

class Editor
{
public:
	Editor(Level *level);
	~Editor();

	void selectPoint(Player *player, int x, int y, int mapRotate, double mapZoom);
	void movePoints(Player *player, int x, int y, int mapRotate, double mapZoom);
	Point *selectedPoint();

protected:
	Point *mPoints[4];
	Point mSelectedPoint;
	bool mPointIsSelected;
	Level *mLevel;
};

#endif