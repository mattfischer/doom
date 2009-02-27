#include "Edit.h"

#include "Game.h"
#include "World.h"
#include "Level.h"

Editor::Editor(Level *level)
{
	mLevel = level;

	mPoints[0] = mPoints[1] = mPoints[2] = mPoints[3] = NULL;
	mPointIsSelected = false;
}

Editor::~Editor()
{
}

void Editor::selectPoint(Player *player, int x, int y, int mapRotate, double mapZoom)
{
	int i, j;
	int x0, y0;

	mPoints[0] = mPoints[1] = mPoints[2] = mPoints[3] = NULL;
	mPointIsSelected = false;

	for(i=0; i<mLevel->numSectors; i++)
	{
		Sector *sector = &mLevel->sectors[i];

		for(j=0; j<sector->numWalls; j++)
		{
			x0 = (sector->walls[j].start.x - player->x) * mapZoom + 320;
			y0 = -(sector->walls[j].start.y - player->y) * mapZoom + 240;
			if(abs(x0 - x)<3 && abs(y0 - y)<3)
			{
				mPoints[0] = &(sector->walls[j].start);
				mPoints[1] = &(sector->walls[(j + sector->numWalls - 1) % sector->numWalls].end);
				mSelectedPoint.x = x0;
				mSelectedPoint.y = y0;
				mPointIsSelected = true;
				if(sector->walls[j].flags&WALL_ADJOINED || sector->walls[(j + 1) % sector->numWalls].flags&WALL_ADJOINED)
				{
					mPoints[2] = &(sector->walls[j].adjoin->walls[sector->walls[j].mirror].end);
					mPoints[3] = &(sector->walls[j].adjoin->walls[(sector->walls[j].mirror + sector->walls[j].adjoin->numWalls - 1) % sector->walls[j].adjoin->numWalls].start);
				}
			}
			if(mPointIsSelected) break;
		}
		if(mPointIsSelected) break;
	}
}

void Editor::movePoints(Player *player, int x, int y, int mapRotate, double mapZoom)
{
	float x1, y1;

	if(!mPointIsSelected) return;

	x1=((float)(x - 320)) / mapZoom + player->x;
	y1=((float)(240 - y)) / mapZoom + player->y;

	for(int i=0; i<4; i++)
	{
		if(mPoints[i] != NULL) 
		{
			mPoints[i]->x = x1;
			mPoints[i]->y = y1;
		}
	}
}

Point *Editor::selectedPoint()
{
	if(mPointIsSelected)	return &mSelectedPoint;
	else					return NULL;
}
	