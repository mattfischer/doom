#include "Renderer.h"
#include "World.h"
#include "Texture.h"
#include "Level.h"
#include "Debug.h"
#include "GraphicsContext.h"
#include "Draw.h"
#include "TexturingEngine.h"

#include <math.h>

void Renderer::getFloorUV(int y, Sector *sector, Player *player, double px, double py, double *u, double *v)
{
	double mpy, yp, s;
	double lpx, lpy, ix, iy, ipx, ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	mpy = player->height - sector->floorheight;
	yp = (double)(y - mHorizon) / (mContext->height() * 2);
	s = mpy / yp;
		
	ix = px * s + p0x;
	iy = py * s + p0y;

	ipx = ix - sector->walls->start.x;
	ipy = sector->walls->start.y - iy;
	
	*u = ipx * WORLDTOTEX;
	*v = ipy * WORLDTOTEX;
}

void Renderer::getCeilingUV(int y, Sector *sector, Player *player, double px, double py, double *u, double *v)
{
	double mpy, yp, s;
	double lpx, lpy, ix, iy, ipx, ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	mpy = sector->ceilingheight - player->height;
	yp = (double)(mHorizon - y) / (mContext->height() * 2);
	s = mpy / yp;
		
	ix = px * s + p0x;
	iy = py * s + p0y;

	ipx = ix - sector->walls->start.x;
	ipy = sector->walls->start.y - iy;
	
	*u = ipx * WORLDTOTEX;
	*v = ipy * WORLDTOTEX;
}


void Renderer::drawMap(Player *player, int rotate, double zoom, Point *selectedPoint)
{
	int i,j;
	struct Sector *sector;
	struct Wall *wall;
	int x0, y0, x1, y1;
	int x0b, y0b, x1b, y1b;
	int x0c, y0c, x1c, y1c;
	int cx = mContext->width() / 2;
	int cy = mContext->height() / 2;

	if(rotate == 0)	DrawLine(mContext, cx, cy, cx + cosp * 20, cy - sinp * 20, 0xFF0000);
	else			DrawLine(mContext, cx, cy, cx, cy - 20, 0xFF0000);
	DrawCircle(mContext, cx, cy, 7, 0xFF0000);

	for(i=0; i<mLevel->numSectors; i++)
	{
		sector = &mLevel->sectors[i];
		for(j=0; j<sector->numWalls; j++)
		{
			wall = &sector->walls[j];
			x0b = (wall->start.x - player->x) * zoom + cx;
			y0b = -(wall->start.y - player->y) * zoom + cy;
			x1b = (wall->end.x - player->x) * zoom + cx;
			y1b = -(wall->end.y - player->y) * zoom + cy;

			if(rotate)
			{
				x0c = x0b - cx;
				y0c = y0b - cy;
				x1c = x1b - cx;
				y1c = y1b - cy;
	
				x0 = cx + x0c * sinp + y0c * cosp;
				y0 = cy + y0c * sinp - x0c * cosp;
				x1 = cx + x1c * sinp + y1c * cosp;
				y1 = cy + y1c * sinp - x1c * cosp;
			}
			else
			{
				x0 = x0b;
				y0 = y0b;
				x1 = x1b;
				y1 = y1b;
			}
	
			if(wall->flags == WALL_NORMAL)	DrawLine(mContext, x0, y0, x1, y1, 0x00FF00);
			else							DrawLine(mContext, x0, y0, x1, y1, 0x008000);

			int i,j;
			DWORD color;

			if(selectedPoint && selectedPoint->x==x0 && selectedPoint->y==y0)	color=0xFFFFFF;
			else																color=0xFFFF00;
			for(i=x0-2; i<x0+2; i++)
				for(j=y0-2; j<y0+2; j++)
					PutPixel(mContext, i, j, color);
		}
	}
}

void Renderer::drawWallSlice(Player *player, int x, int ignore, int miny, int maxy)
{
	struct Wall tempwall;
	double q0x, q0y, q1x, q1y;
	double qx, qy, pqx, pqy;
	double s, d, bottom, dsd;
	double ix, iy;
	double u, v;
	int h1t, h2t, h1bt, h2bt;
	int h1i, h2i, h1bi, h2bi;
	int wall;
	int toptx, midtx, botttx;
	Texture *toptex, *midtex, *botttex;
	int keepgoing=1;
	int k;
	int i;
    
	Sector *sector = player->sector;

	while(keepgoing)
	{
		if(sector->lastwall != -1)	wall = sector->lastwall-1;
		else						wall = -1;
		
		for(i=0; i<sector->numWalls; i++)
		{
			wall++;		
			if(wall >= sector->numWalls) wall %= sector->numWalls;
			if(wall == ignore) continue;
			tempwall = sector->walls[wall];
			
			q0x = tempwall.start.x;
			q0y = tempwall.start.y;
			q1x = tempwall.end.x;
			q1y = tempwall.end.y;
			
			qx = q1x - q0x;
			qy = q1y - q0y;
			pqx = p0x - q0x;
			pqy = p0y - q0y;
			
			bottom = px * qy - py * qx;
			s = (px * pqy - py * pqx) / bottom;
			if(s<0.0 || s>1.0) continue;
		
			ix = q0x + qx * s;
			iy = q0y + qy * s;
			if(mUsePx) d = ix - p0x;
			else   	  d = iy - p0y;
			
			dsd = ds / d;
			if(dsd > 0.0) break;
		}

		h1i = mHorizon + (int)(dsd * (player->height - sector->floorheight));
		h2i = mHorizon - (int)(dsd * (sector->ceilingheight - player->height));
		
		if(sector->beenin != mGlobalFlag)
		{
			sector->fy = maxy;
			sector->fy2 = maxy;

			sector->cy = miny;
			sector->cy2 = miny;
			sector->lastwall = -1;
			
			sector->beenin = mGlobalFlag;
		}
	
		// Floor cases:  A floor segment is "opened" when its scanline is started, and "closed" when it is finished
		// Five different actions can cause a floor scanline to open or close: the wall can move higher/lower than it was 
		// in the last column, the viewport into the sector (when rendering an adjoined sector) can move higher/lower, or a
		// non-adjoined wall can completely seal off the view into an adjoined sector.
		//
		// View into a room which has a stairstep-like adjoin on both side walls:
		//  |       |        |      |      
		//  |_______|        |______|      
		//  |      /|        |\     |     
		//  F4 - F3 |        | F5 - F1     
		//  |    /  |        |  \   |      
		//  |   /   |________|   \  |     
		//  |  /   /         \    \ |      
		//  | /   /           \    \|      
		//  |/   F4 - - - - - F2    |      
		//  |   /               \   |      
		//  |  /                 \  |      
		//  | /                   \ |      
		//  |/_____________________\|      

		if(tempwall.flags == WALL_NORMAL)
		{
			// F1: floor segments in other sectors closed by the beginning of a non-adjoined wall
			for(k=fy+1; k<=sector->fy; k++)
			{
				if(k < miny) continue;
				if(k < mHorizon) continue;
				if(k > maxy) break;
				
				if(mFloorCeilingInfo[k].x == -1) continue;

				getFloorUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
				mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				mFloorCeilingInfo[k].x = -1;
			}
			fy = sector->fy;
		}

		if(h1i > sector->fy)
		{
			// F2: floor segments closed by a wall bottom going further down the screen than the last column
			for(k=sector->fy+1; k<=h1i; k++)
			{
				if(k < miny) continue;
				if(k < mHorizon) continue;
				if(k > maxy) break;
				if(mFloorCeilingInfo[k].x == -1) continue;

				getFloorUV(k, sector, player, px, py, &u, &v);
				mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				mFloorCeilingInfo[k].x = -1;
			}
			sector->fy = h1i;
			if(tempwall.flags == WALL_NORMAL) fy = h1i;
		}

		if(maxy < sector->fy2)
		{
			// F3: floor segments closed by maxy going further up the screen than the last column
			for(k=maxy+1; k<=sector->fy2; k++)
			{
				if(mFloorCeilingInfo[k].x == -1) continue;
				if(mFloorCeilingInfo[k].sector != sector) continue;
				
				if(k > mHorizon)
				{
					getFloorUV(k, sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}
				else
				{
					getCeilingUV(k, sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}
				mFloorCeilingInfo[k].x = -1;
			}
			sector->fy2 = maxy;
		}

		if(h1i < sector->fy)
		{
			// F4: floor segments opened by the wall bottom going further up the screen than the last column
			for(k=h1i+1; k<=sector->fy; k++)
			{
				if(k < miny) continue;
				if(k < mHorizon) continue;
				if(k > maxy) break;

				// close any previous segments on this scanline
				if(mFloorCeilingInfo[k].x != -1)
				{
					getFloorUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}

				mFloorCeilingInfo[k].x = x;
				mFloorCeilingInfo[k].sector = sector;
				getFloorUV(k, sector, player, px, py, &u, &v);
				mFloorCeilingInfo[k].u = u;
				mFloorCeilingInfo[k].v = v;
			}
			sector->fy = h1i;
			fy = h1i;
		}

		if(maxy > sector->fy2)
		{
			// F5: floor segments opened by maxy going further down the screen than the last column
			for(k=sector->fy2+1; k<=maxy; k++)
			{
				if(k < miny) continue;
				if(k<mHorizon && maxy>=h2i) continue;

				mFloorCeilingInfo[k].x = x;
				mFloorCeilingInfo[k].sector = sector;
				if(k > mHorizon) getFloorUV(k, sector, player, px, py, &u, &v);
				else getCeilingUV(k, sector, player, px, py, &u, &v);
				
				mFloorCeilingInfo[k].u = u;
				mFloorCeilingInfo[k].v = v;
			}
			sector->fy2 = maxy;
		}
	
		// end floor

		// Ceiling cases are mirror images of floor cases - see above

		if(tempwall.flags == WALL_NORMAL)
		{
			// C1: ceiling segments closed by the beginning of a non-adjoined wall
			for(k=cy-1; k>=sector->cy; k--)
			{
				if(k < miny) break;
				if(k > mHorizon) continue;
				if(k > maxy) continue;

				if(mFloorCeilingInfo[k].x == -1) continue;

				getCeilingUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
				mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				mFloorCeilingInfo[k].x = -1;
			}
			cy = sector->cy;
		}

		if(h2i < sector->cy)
		{
			// C2: ceiling segments closed by the wall top going further up the screen than the last column
			for(k=sector->cy-1; k>=h2i; k--)
			{
				if(k < miny) break;
				if(k > mHorizon) continue;
				if(k > maxy) continue;
				if(mFloorCeilingInfo[k].x == -1) continue;

				getCeilingUV(k, sector, player, px, py, &u, &v);
				mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				mFloorCeilingInfo[k].x = -1;
			}
			sector->cy = h2i;
			if(tempwall.flags == WALL_NORMAL) cy = h2i;
		}

		if(miny > sector->cy2)
		{
			// C3: ceiling segments closed by miny going farther down the screen than the last column
			for(k=miny-1; k>=sector->cy2; k--)
			{
				if(mFloorCeilingInfo[k].x == -1) continue;
				if(mFloorCeilingInfo[k].sector != sector) continue;

				if(k < mHorizon)
				{
					getCeilingUV(k, sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}
				else
				{
					getFloorUV(k, sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}
				mFloorCeilingInfo[k].x = -1;
			}
			sector->cy2 = miny;
		}

		if(h2i > sector->cy)
		{
			// C4: ceiling segments opened by the wall top going further down the screen than the last column
			for(k=h2i-1; k>=sector->cy; k--)
			{
				if(k < miny) break;
				if(k > mHorizon) continue;
				if(k > maxy) continue;

				// close any existing segment
				if(mFloorCeilingInfo[k].x != -1)
				{
					getCeilingUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
					mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x - 1, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
				}

				mFloorCeilingInfo[k].x = x;
				mFloorCeilingInfo[k].sector = sector;
				getCeilingUV(k, sector, player, px, py, &u, &v);
				mFloorCeilingInfo[k].u = u;
				mFloorCeilingInfo[k].v = v;
			}
			sector->cy = h2i;
			cy = h2i;
		}

		if(miny < sector->cy2)
		{
			// C5: ceiling segments opened by miny going further up the screen than it was in the last column
			for(k=sector->cy2-1; k>=miny; k--)
			{
				if(k < miny) break;
				if(k>mHorizon && miny<=h1i) continue;

				mFloorCeilingInfo[k].x = x;
				mFloorCeilingInfo[k].sector = sector;
				if(k < mHorizon) getCeilingUV(k, sector, player, px, py, &u, &v);
				else getFloorUV(k, sector, player, px, py, &u, &v);
				mFloorCeilingInfo[k].u = u;
				mFloorCeilingInfo[k].v = v;
			}
			sector->cy2 = miny;
		}
	
		// end ceiling
		

		sector->lastwall = wall;
		
		if(h2i > maxy) return;
		if(h1i < miny) return;
		
		if(tempwall.flags == WALL_NORMAL)
		{
			h2t = tempwall.midoffset;
			h1t = tempwall.midbott;
			midtex = tempwall.midtex;
			midtx = tempwall.midhoriz + (int)(s * tempwall.length * WORLDTOTEX) + tempwall.midmod;
			while(midtx < 0) 
			{ 
				midtx += midtex->width; 
				tempwall.midmod += midtex->width; 
			}
			while(midtx >= midtex->width) 
			{ 
				midtx -= midtex->width; 
				tempwall.midmod -= midtex->width;
			}
			
			mTexturingEngine->textureWall(x, h2i, h1i, midtex, midtx, h2t, h1t, miny, maxy);
			keepgoing = 0;
		}
		
		if(tempwall.flags == WALL_ADJOINED)
		{
			h1bi = mHorizon + (int)(dsd * (player->height - tempwall.adjoin->floorheight));
			h2bi = mHorizon - (int)(dsd * (tempwall.adjoin->ceilingheight - player->height));
						
			if(h2bi >= miny)
			{
				if(h2bi > h2i) 
				{ 
					h2t = tempwall.topoffset;
					h2bt = tempwall.topbott;
					toptex = tempwall.toptex;
					toptx = tempwall.tophoriz + (int)(s * tempwall.length * WORLDTOTEX) + tempwall.topmod;
					
					while(toptx < 0) 
					{ 
						toptx += toptex->width; 
						tempwall.topmod += toptex->width; 
					}
					while(toptx >= toptex->width) 
					{	
						toptx -= toptex->width; 
						tempwall.topmod -= toptex->width;
					}
					mTexturingEngine->textureWall(x, h2i, h2bi, toptex, toptx, h2t, h2bt, miny, maxy);
				}
			}
			else 
			{
				h2bi = miny;
			}

			if(h1bi <= maxy)
			{
				if(h1bi < h1i) 
				{ 
					h1bt = tempwall.bottomoffset;
					h1t = tempwall.bottombott;
					botttex = tempwall.botttex;
					botttx = tempwall.botthoriz + (int)(s * tempwall.length * WORLDTOTEX) + tempwall.bottmod;
					while(botttx < 0) 
					{ 
						botttx += botttex->width; 
						tempwall.bottmod += botttex->width; 
					}
					while(botttx >= botttex->width) 
					{ 
						botttx -= botttex->width; 
						tempwall.bottmod -= botttex->width;
					}
					
					mTexturingEngine->textureWall(x, h1bi, h1i, botttex, botttx, h1bt, h1t, miny, maxy);
				}	
			}
			else 
			{
				h1bi = maxy;
			}
	
			if(h1bi > h1i) h1bi = h1i;
			if(h2bi < h2i) h2bi = h2i;
		
			if(h2bi>=maxy || h1bi<=miny) keepgoing = 0;
			sector = tempwall.adjoin;
			ignore = tempwall.mirror;
			miny = h2bi;
			maxy = h1bi;
		}
	}
}

void Renderer::drawWalls(Player *player)
{
	int x;
	double xinc, yinc;
	double u, v;
	double FOV = 60;
	double w = sin (FOV * 3.14 / 360) * 2;

	cy = 0;
	fy = mContext->height() - 1;
	
	p0x = player->x;
	p0y = player->y;
	p1x = cosp - sinp * w / 2 + p0x;
	p1y = sinp + cosp * w / 2 + p0y;
	xinc = w / mContext->width() * sinp;
	yinc = -w / mContext->width() * cosp;
	
	for(x=0; x<mContext->width(); x++) 
	{
		px = p1x - p0x;
		py = p1y - p0y;
		
		if(abs(px) > abs(py))
		{ 
			mUsePx = true;
			ds = mContext->height() * 2* px; 
		}
		else
		{ 
			mUsePx = false;
			ds = mContext->height() * 2* py; 
		}

		drawWallSlice(player, x, -1, 0, mContext->height() - 1);
		
		p1x += xinc;
		p1y += yinc;		
	}
	int k;

	for(k=fy+1; k<mContext->height(); k++)
	{
		if(k < 0) continue;
		if(mFloorCeilingInfo[k].x == -1) continue;

		getFloorUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
		mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x, k, mFloorCeilingInfo[k].sector->floortex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
		mFloorCeilingInfo[k].x = -1;
	}

	for(k=cy-1; k>=0; k--)
	{
		if(k > mContext->height() - 1) continue;
		if(mFloorCeilingInfo[k].x == -1) continue;

		getCeilingUV(k, mFloorCeilingInfo[k].sector, player, px, py, &u, &v);
		mTexturingEngine->textureFloorCeiling(mFloorCeilingInfo[k].x, x, k, mFloorCeilingInfo[k].sector->ceiltex, mFloorCeilingInfo[k].u, mFloorCeilingInfo[k].v, u, v);
		mFloorCeilingInfo[k].x = -1;
	}

	if(mGlobalFlag < 65536) mGlobalFlag++;
	else mGlobalFlag = 0;
}	

void Renderer::drawScreen(Player *player)
{
	sinp = sin(player->angle);
	cosp = cos(player->angle);
	drawWalls(player);
}

Renderer::Renderer(GraphicsContext *context, Level *level)
{
	mContext = context;
	mLevel = level;
	mGlobalFlag = 1;

	mFloorCeilingInfo = new FloorCeilingInfo[mContext->height()];

	mTexturingEngine = TexturingEngine::createTexturingEngine(mContext);

	int i;
	for(i=0; i<mContext->height(); i++) mFloorCeilingInfo[i].x=-1;
}

Renderer::~Renderer()
{
	delete[] mFloorCeilingInfo;
}

void Renderer::setHorizon(int horizon)
{
	mHorizon = horizon;
}

int Renderer::horizon()
{
	return mHorizon;
}