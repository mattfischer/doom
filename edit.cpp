#include "edit.h"
#include "Game.h"
#include "World.h"
#include "Level.h"

Point *point1=NULL;
Point *point2=NULL;
Point *point3=NULL;
Point *point4=NULL;
Point screenpoint;
bool pointselected;

void SelectPoint(Player *player, Level *level, MapInfo *mapInfo, int x, int y)
{
	int i, j;
	int x0, y0;

	point1 = NULL;
	point2 = NULL;
	point3 = NULL;
	point4 = NULL;
	pointselected = false;

	for(i=0; i<level->numSectors; i++)
	{
		Sector *sector = &level->sectors[i];

		for(j=0; j<sector->numWalls; j++)
		{
			x0 = (sector->walls[j].start.x - player->x) * mapInfo->zoom + 320;
			y0 = -(sector->walls[j].start.y - player->y) * mapInfo->zoom + 240;
			if(abs(x0 - x)<3 && abs(y0 - y)<3)
			{
				point1 = &(sector->walls[j].start);
				point2 = &(sector->walls[(j + sector->numWalls - 1) % sector->numWalls].end);
				screenpoint.x = x0;
				screenpoint.y = y0;
				pointselected = true;
				if(sector->walls[j].flags&WALL_ADJOINED || sector->walls[(j + 1) % sector->numWalls].flags&WALL_ADJOINED)
				{
					point3 = &(sector->walls[j].adjoin->walls[sector->walls[j].mirror].end);
					point4 = &(sector->walls[j].adjoin->walls[(sector->walls[j].mirror + sector->walls[j].adjoin->numWalls - 1) % sector->walls[j].adjoin->numWalls].start);
				}
			}
			if(pointselected) break;
		}
		if(pointselected) break;
	}
}

void MovePoints(Player *player, MapInfo *mapInfo, int x, int y)
{
	float x1, y1;

	if(!pointselected) return;

	x1=((float)(x - 320)) / mapInfo->zoom + player->x;
	y1=((float)(240 - y)) / mapInfo->zoom + player->y;

	if(point1 != NULL) 
	{
		point1->x = x1;
		point1->y = y1;
	}
	if(point2 != NULL) 
	{
		point2->x = x1;
		point2->y = y1;
	}
	if(point3 != NULL) 
	{
		point3->x = x1;
		point3->y = y1;
	}
	if(point4 != NULL) 
	{
		point4->x = x1;
		point4->y = y1;
	}
}