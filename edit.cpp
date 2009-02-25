#include "global.h"
#include "edit.h"

void SelectPoint(int x, int y)
{
	int i, j;
	int x0, y0;

	point1 = NULL;
	point2 = NULL;
	point3 = NULL;
	point4 = NULL;
	pointselected = false;

	for(i=0; i<numsectors; i++)
	{
		for(j=0; j<sectors[i].numwalls; j++)
		{
			x0 = (sectors[i].walls[j].start.x - player.x) * mapinfo.zoom + 320;
			y0 = -(sectors[i].walls[j].start.y - player.y) * mapinfo.zoom + 240;
			if(abs(x0 - x)<3 && abs(y0 - y)<3)
			{
				point1 = &(sectors[i].walls[j].start);
				point2 = &(sectors[i].walls[(j + sectors[i].numwalls - 1) % sectors[i].numwalls].end);
				screenpoint.x = x0;
				screenpoint.y = y0;
				pointselected = true;
				if(sectors[i].walls[j].flags&WALL_ADJOINED || sectors[i].walls[(j + 1) % sectors[i].numwalls].flags&WALL_ADJOINED)
				{
					point3 = &(sectors[i].walls[j].adjoin->walls[sectors[i].walls[j].mirror].end);
					point4 = &(sectors[i].walls[j].adjoin->walls[(sectors[i].walls[j].mirror + sectors[i].walls[j].adjoin->numwalls - 1) % sectors[i].walls[j].adjoin->numwalls].start);
				}
			}
			if(pointselected) break;
		}
		if(pointselected) break;
	}
}

void MovePoints(int x, int y)
{
	float x1, y1;

	x1=((float)(x - 320)) / mapinfo.zoom + player.x;
	y1=((float)(240 - y)) / mapinfo.zoom + player.y;

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