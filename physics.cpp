#include "global.h"
#include "physics.h"
#include "World.h"
#include "Level.h"

#include <math.h>

void DoWallCollisions(Player *player, double newx, double newy)
{
	Sector *sector;
	Wall *wall;
	double px, py, perx, pery, parx, pary;
	double qx, qy;
	double p0x, p0y, p1x, p1y;
	double q0x, q0y, q1x, q1y;
	double pqx, pqy;
	
	double s, t;
	double dper, dpwall;
	int i;
	char buffer[100];
	bool repeat = true;
	int *walllist;
	Sector *newsector;

	sector = player->sector;
	newsector = player->sector;
	walllist = new int[sector->numWalls];
	for(i=0; i<sector->numWalls; i++) walllist[i] = 0;
	
	px = newx - player->x;
	py = newy - player->y;
	while(repeat)
	{
		px = newx - player->x;
		py = newy - player->y;
		
		p0x = player->x;
		p0y = player->y;
		p1x = newx;
		p1y = newy;

		
		for(i=0; i<sector->numWalls; i++)
		{
			repeat = 0;
			wall = &sector->walls[i];
			if(walllist[i] == 1) continue;
			q0x = wall->start.x;
			q0y = wall->start.y;
			q1x = wall->end.x;
			q1y = wall->end.y;

			pqx = p0x - q0x;
			pqy = p0y - q0y;

			qx = q1x - q0x;
			qy = q1y - q0y;

			parx = qx * (px * qx + py * qy) / (qx * qx + qy * qy);
			pary = qy * (px * qx + py * qy) / (qx * qx + qy * qy);
			perx = px - parx;
			pery = py - pary;
			
			s = (perx * pqy - pqx * pery) / (perx * qy - qx * pery);
			t = (pqx * qy - qx * pqy) / (perx * qy - qx * pery);

			
			if(wall->flags == WALL_NORMAL) 
				if(s < -wall->soffset || s > 1.0 + wall->soffset) continue;
			
			if(wall->flags == WALL_ADJOINED) 
				if(s < wall->soffset || s > 1.0 - wall->soffset) continue;

			if(t > 0) continue;
			dpwall = -(qx * pqy - qy * pqx) / sqrt(qx * qx + qy * qy);
			dper = sqrt(perx * perx + pery * pery);
			
			if(dpwall - dper < 2.0)
			{
				walllist[i] = 1;
				if(wall->flags == WALL_ADJOINED)
				{
					if(wall->adjoin->floorheight - (player->height - player->fromfloor) < 1)
						if(wall->adjoin->ceilingheight - wall->adjoin->floorheight > player->fromfloor)
						{
							if(dpwall - dper > 0) continue;
												
							player->sector = wall->adjoin;
							newsector = wall->adjoin;
							player->height = wall->adjoin->floorheight + player->fromfloor;
							repeat = true;
							break;
							
						}
				}
				
				if(dpwall >= 2.0)
				{
					newx = (dpwall - 2.0) * perx / dper + parx + p0x;
					newy = (dpwall - 2.0) * pery / dper + pary + p0y;
				}
				else
				{
					newx = parx + p0x;
					newy = pary + p0y;
				}
				repeat = 1;
				break;
			}
		}
	}

	player->x = newx;
	player->y = newy;	
}