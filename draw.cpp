#include <math.h>
#include "global.h"
#include "macros.h"
#include "draw.h"

#include "World.h"
#include "Texture.h"
#include "Level.h"
#include "Debug.h"
#include "DDraw.h"

int showmap=0;
int rotatemap=0;

double sinp, cosp;	
double ds0=1.5;
double w=1.25;
double w2=.625;
double w640=.001953295;
double hw=512;	
double px,py,p0x,p0y,p1x,p1y,ds;
int usepx;
int globalflag=1;

double g_u, g_v;

int fy;
int cy;

int horizon = VSIZE/2;

extern Point screenpoint;
extern bool pointselected;

void PutPixel( GraphicsContext *context, int x, int y, DWORD color)
{
	if(x<0 || x>HSIZE-1 || y<0 || y>VSIZE-1) return;

	UCHAR *frameBuffer = context->frameBuffer();
	int pitch = context->pitch();

	*(frameBuffer + y * pitch + x * 4) = color&0xFF;
	*(frameBuffer + y * pitch + x * 4 + 1) = (color>>8)&0xFF;
	*(frameBuffer + y * pitch + x * 4 + 2) = (color>>16)&0xFF;
}

void vertex(GraphicsContext *context, int x, int y)
{
	int i,j;
	DWORD color;

	if(pointselected && screenpoint.x==x && screenpoint.y==y) color=0xFFFFFF;
	else									 color=0xFFFF00;
	for(i=x-2; i<x+2; i++)
		for(j=y-2; j<y+2; j++)
			PutPixel(context, i, j, color);
}
void line(GraphicsContext *context, int x0, int y0, int x1, int y1, DWORD color)
{
	int xd,yd;
	int error;
	int x,y;
	int xi1,xi2,yi1,yi2;
	int x0c,y0c,x1c,y1c;
	int e1,e2;
	int i,i2;
	
	xd = abs(x1 - x0);
	yd = abs(y1 - y0);
	x = x0;
	y = y0;
	
	if(x1 > x0) xi1 = 1; else xi1 = -1;
	if(y1 > y0) yi1 = 1; else yi1 = -1;
	
	i = 0;
	if(xd > yd) 
	{ 
		i2 = xd;
		error = xd / 2;
		e1 = yd;
		e2 = xd;
		xi2 = xi1; 
		yi2 = 0;
	} 
	else
	{
		i2 = yd;
		error = yd/2;
		e1 = xd;
		e2 = yd;
		xi2 = 0;
		yi2 = yi1;
	}

	while(i != i2)
	{
		PutPixel(context, x, y, color);
		error += e1;
		if(error > e2)
		{
			error -= e2;
			x += xi1;
			y += yi1;
		}
		else 
		{
			x += xi2;
			y += yi2;
		}
		i++;
	}
}

void circle(GraphicsContext *context, int cx, int cy, int rad, DWORD color)
{
	int d;
	int x,y;

	d = 3 - (2 * rad);
	x = 0;
	y = rad;

	while(1)
	{
		PutPixel(context, cx + x, cy + y, color);
		PutPixel(context, cx - x, cy + y, color);
		PutPixel(context, cx + x, cy - y, color);
		PutPixel(context, cx - x, cy - y, color);
		PutPixel(context, cx + y, cy + x, color);
		PutPixel(context, cx + y, cy - x, color);
		PutPixel(context, cx - y, cy + x, color);
		PutPixel(context, cx - y, cy - x, color);

		if(x==y || x==rad) break;
		if(d < 0) d += 4 * x + 6;
		else
		{
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

void vline(GraphicsContext *context, int x, int y0, int y1, DWORD color)
{
	int y;
	DWORD val;
	UCHAR *frameBuffer = context->frameBuffer();
	int pitch = context->pitch();

	val = y0 * pitch;
	for(y=y0; y<=y1; y++)
	{
		*(frameBuffer + val + 4 * x) = (UCHAR)color&0xFF;
		*(frameBuffer + val + 4 * x + 1) = (UCHAR)(color>>8)&0xFF;
		*(frameBuffer + val + 4 * x + 2) = (UCHAR)(color>>16)&0xFF;
		val += pitch;
	}
}

unsigned long dofade(unsigned long color,int fade)
{
	unsigned char r,g,b;

	r=color&0xFF;
	g=(color>>8)&0xFF;
	b=(color>>16)&0xFF;
	if(r>fade) r=r-fade;
	else r=0;
	
	if(g>fade) g=g-fade;
	else g=0;
	
	if(b>fade) b=b-fade;
	else b=0;

	color=b;
	color<<=8;
	color|=g;
	color<<=8;
	color|=r;

	return color;
}

static void texwall_ref(GraphicsContext *context, int x, int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy, int fade)
{
#ifdef NOVIS
	return;
#endif

	if(tx < 0) tx += texture->width * (abs(tx) / texture->width + 1);
	tx %= texture->width;

	for(int y = 0; y <= y1 - y0; y++) 
	{
		if(y + y0 < miny) continue;
		if(y + y0 > maxy) continue;

		int ty = ty0 + y * (ty1 - ty0 - 1) / (y1 - y0);
		if(ty < 0) ty += texture->height * (abs(ty)/ texture->height + 1);
		ty %= texture->height;

		UCHAR *dst = context->frameBuffer() + (y + y0) * context->pitch() + x * 4;
		UCHAR *src = texture->data + ty * texture->width * 3 + tx * 3;

		*dst = *src;
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
}

void Renderer::textureWall(int x,int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy, int fade)
{
	texwall_ref(mContext, x, y0, y1, texture, tx, ty0, ty1, miny, maxy, fade);
}

void hline(GraphicsContext *context, int x0, int x1, int y, DWORD color)
{
	int x;
	DWORD val;
		
#ifdef NOVIS
	return;
#endif

	val = y * context->pitch();
	for(x=x0; x<=x1; x++)
	{
		*(context->frameBuffer() + val + x * 4) = (UCHAR)color&0xFF;
		*(context->frameBuffer() + val + x * 4 + 1) = (UCHAR)(color>>8)&0xFF;
		*(context->frameBuffer() + val + x * 4 + 2) = (UCHAR)(color>>16)&0xFF;
	}

  }

void getuvfloor(int y, Sector *sector, Player *player, double data1, double data2, int datatype)
{
	double mpy, yp, s;
	double lpx, lpy, ix, iy, ipx, ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	if(datatype == 0)
	{
		ix = data1;
		iy = data2;
	}
	else
	{
		mpy = player->height - sector->floorheight;
		yp = (y - horizon) / hw;
		s = mpy / yp;
			
		ix = data1 * s + p0x;
		iy = data2 * s + p0y;
	}

	ipx = ix - sector->walls->start.x;
	ipy = sector->walls->start.y - iy;
	
	g_u = ipx * WORLDTOTEX;
	g_v = ipy * WORLDTOTEX;
}

void getuvceil(int y, Sector *sector, Player *player, double data1, double data2, int datatype)
{
	double mpy, yp, s;
	double lpx, lpy, ix, iy, ipx, ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	if(datatype == 0)
	{
		ix=data1;
		iy=data2;
	}
	else
	{
		mpy = sector->ceilingheight - player->height;
		yp = (horizon - y) / hw;
		s = mpy / yp;
			
		ix = data1 * s + p0x;
		iy = data2 * s + p0y;
	}

	ipx = ix - sector->walls->start.x;
	ipy = sector->walls->start.y - iy;
	
	g_u = ipx * WORLDTOTEX;
	g_v = ipy * WORLDTOTEX;
}

static void texfloorceil_ref(GraphicsContext *context, int x0, int x1, int y, struct Sector *sector, Texture *texture, double u0, double v0, double u1, double v1)
{
	int u,v;
	int x;
	UCHAR *src, *dst;
	int width3;

	if(x1 < x0) return;
	
	width3 = texture->width * 3;
	dst = context->frameBuffer() + y * context->pitch() + x0 * 4;

	for(x=0; x <= x1-x0; x++)
	{
		u = u0 + x * (u1 - u0) / (x1 - x0);
		v = v0 + x * (v1 - v0) / (x1 - x0);

		if(u < 0) u += texture->width * (abs(u) / texture->width + 1);
		if(v < 0) v += texture->height * (abs(v) / texture->height + 1);
		u %= texture->width;
		v %= texture->height;
		src = texture->data + v * width3 + u * 3;

		*(dst) = *(src);
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
		
		dst += 4;
	}
}

void Renderer::textureFloorCeiling(int x0, int x1, int y, struct Sector *sector, Texture *texture, double u0, double v0, double u1, double v1)
{
	texfloorceil_ref(mContext, x0, x1, y, sector, texture, u0, v0, u1, v1);
}

void Renderer::drawMap(MapInfo *mapInfo)
{
	int i,j;
	struct Sector *sector;
	struct Wall *wall;
	int x0, y0, x1, y1;
	int x0b, y0b, x1b, y1b;
	int x0c, y0c, x1c, y1c;
	
	if(mapInfo->rotate == 0)	line(mContext, 320, 240, 320 + cosp * 20, 240 - sinp * 20, 0xFF0000);
	else						line(mContext, 320, 240, 320, 220, 0xFF0000);
	circle(mContext, 320, 240, 7, 0xFF0000);

	for(i=0; i<mLevel->numSectors; i++)
	{
		sector = &mLevel->sectors[i];
		for(j=0; j<sector->numWalls; j++)
		{
			wall = &sector->walls[j];
			x0b = (wall->start.x - mLevel->player->x) * mapInfo->zoom + 320;
			y0b = -(wall->start.y - mLevel->player->y) * mapInfo->zoom + 240;
			x1b = (wall->end.x - mLevel->player->x) * mapInfo->zoom + 320;
			y1b = -(wall->end.y - mLevel->player->y) * mapInfo->zoom + 240;

			if(mapInfo->rotate)
			{
				x0c = x0b - 320;
				y0c = y0b - 240;
				x1c = x1b - 320;
				y1c = y1b - 240;
	
				x0 = 320 + x0c * sinp + y0c * cosp;
				y0 = 240 + y0c * sinp - x0c * cosp;
				x1 = 320 + x1c * sinp + y1c * cosp;
				y1 = 240 + y1c * sinp - x1c * cosp;
			}
			else
			{
				x0 = x0b;
				y0 = y0b;
				x1 = x1b;
				y1 = y1b;
			}
	
			if(wall->flags == WALL_NORMAL)	line(mContext, x0, y0, x1, y1, 0x00FF00);
			else							line(mContext, x0, y0, x1, y1, 0x008000);

			vertex(mContext, x0, y0);
		}
	}
}

struct FloorCeilingInfo {
	int x;
	double u;
	double v;
	Sector *sector;
};

FloorCeilingInfo fc[VSIZE];

void Renderer::drawWallSlice(Player *player, int x, int ignore, int miny, int maxy)
{
	struct Wall tempwall;
	double q0x, q0y, q1x, q1y;
	double qx, qy, pqx, pqy;
	double s, d, bottom, dsd;
	double ix, iy;
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
			if(usepx) d = ix - p0x;
			else   	  d = iy - p0y;
			
			dsd = ds / d;
			if(dsd > 0.0) break;
		}

		h1i = horizon + (int)(dsd * (player->height - sector->floorheight));
		h2i = horizon - (int)(dsd * (sector->ceilingheight - player->height));
		
		if(sector->beenin != globalflag)
		{
			sector->fy = maxy;
			sector->fy2 = maxy;

			sector->cy = miny;
			sector->cy2 = miny;
			sector->lastwall = -1;
			
			sector->beenin = globalflag;
		}
	
		// begin floor 
		
		if(tempwall.flags == WALL_NORMAL)
		{
			for(k=fy+1; k<=sector->fy; k++)
			{
				if(k < miny) continue;
				if(k < horizon) continue;
				if(k > maxy) break;
				
				if(fc[k].x == -1) continue;

				getuvfloor(k, fc[k].sector, player, px, py, 1);
				textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
				fc[k].x = -1;
			}
			fy = sector->fy;
		}

		if(h1i > sector->fy)
		{
			for(k=sector->fy+1; k<=h1i; k++)
			{
				if(k < miny) continue;
				if(k < horizon) continue;
				if(k > maxy) break;
				if(fc[k].x == -1) continue;

				getuvfloor(k, sector, player, px, py, 1);
				textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
				fc[k].x = -1;
			}
			sector->fy = h1i;
			if(tempwall.flags == WALL_NORMAL) fy = h1i;
		}

		if(maxy < sector->fy2)
		{
			for(k=maxy+1; k<=sector->fy2; k++)
			{
				if(fc[k].x == -1) continue;
				if(fc[k].sector != sector) continue;
				
				if(k > horizon)
				{
					getuvfloor(k, sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
				}
				else
				{
					getuvceil(k, sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
				}
				fc[k].x = -1;
			}
			sector->fy2 = maxy;
		}

		if(h1i < sector->fy)
		{
			for(k=h1i+1; k<=sector->fy; k++)
			{
				if(k < miny) continue;
				if(k < horizon) continue;
				if(k > maxy) break;

				if(fc[k].x != -1)
				{
					getuvfloor(k, fc[k].sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
				}

				fc[k].x = x;
				fc[k].sector = sector;
				getuvfloor(k, sector, player, px, py, 1);
				fc[k].u = g_u;
				fc[k].v = g_v;
			}
			sector->fy = h1i;
			fy = h1i;
		}

		if(maxy > sector->fy2)
		{
			for(k=sector->fy2+1; k<=maxy; k++)
			{
				if(k < miny) continue;
				if(k<horizon && maxy>=h2i) continue;

				fc[k].x = x;
				fc[k].sector = sector;
				if(k > horizon) getuvfloor(k, sector, player, px, py, 1);
				else getuvceil(k, sector, player, px, py, 1);
				
				fc[k].u = g_u;
				fc[k].v = g_v;
			}
			sector->fy2 = maxy;
		}
	
		// end floor

		// begin ceiling

		if(tempwall.flags == WALL_NORMAL)
		{
			for(k=cy-1; k>=sector->cy; k--)
			{
				if(k < miny) break;
				if(k > horizon) continue;
				if(k > maxy) continue;

				if(fc[k].x == -1) continue;

				getuvceil(k, fc[k].sector, player, px, py, 1);
				textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
				fc[k].x = -1;
			}
			cy = sector->cy;
		}

		if(h2i < sector->cy)
		{
			for(k=sector->cy-1; k>=h2i; k--)
			{
				if(k < miny) break;
				if(k > horizon) continue;
				if(k > maxy) continue;
				if(fc[k].x == -1) continue;

				getuvceil(k, sector, player, px, py, 1);
				textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
				fc[k].x = -1;
			}
			sector->cy = h2i;
			if(tempwall.flags == WALL_NORMAL) cy = h2i;
		}

		if(miny > sector->cy2)
		{
			for(k=miny-1; k>=sector->cy2; k--)
			{
				if(fc[k].x == -1) continue;
				if(fc[k].sector != sector) continue;

				if(k < horizon)
				{
					getuvceil(k, sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
				}
				else
				{
					getuvfloor(k, sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
				}
				fc[k].x = -1;
			}
			sector->cy2 = miny;
		}

		if(h2i > sector->cy)
		{
			for(k=h2i-1; k>=sector->cy; k--)
			{
				if(k < miny) break;
				if(k > horizon) continue;
				if(k > maxy) continue;

				if(fc[k].x != -1)
				{
					getuvceil(k, fc[k].sector, player, px, py, 1);
					textureFloorCeiling(fc[k].x, x - 1, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
				}

				fc[k].x = x;
				fc[k].sector = sector;
				getuvceil(k, sector, player, px, py, 1);
				fc[k].u = g_u;
				fc[k].v = g_v;
			}
			sector->cy = h2i;
			cy = h2i;
		}

		if(miny < sector->cy2)
		{
			for(k=sector->cy2-1; k>=miny; k--)
			{
				if(k < miny) break;
				if(k>horizon && miny<=h1i) continue;

				fc[k].x = x;
				fc[k].sector = sector;
				if(k < horizon) getuvceil(k, sector, player, px, py, 1);
				else getuvfloor(k, sector, player, px, py, 1);
				fc[k].u = g_u;
				fc[k].v = g_v;
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
			
			textureWall(x, h2i, h1i, midtex, midtx, h2t, h1t, miny, maxy, d*5);
			keepgoing = 0;
		}
		
		if(tempwall.flags == WALL_ADJOINED)
		{
			h1bi = horizon + (int)(dsd * (player->height - tempwall.adjoin->floorheight));
			h2bi = horizon - (int)(dsd * (tempwall.adjoin->ceilingheight - player->height));
						
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
					textureWall(x, h2i, h2bi, toptex, toptx, h2t, h2bt, miny, maxy, d*5);
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
					
					textureWall(x, h1bi, h1i, botttex, botttx, h1bt, h1t, miny, maxy, d*5);
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

void Renderer::drawWalls()
{
	int x;
	double xinc, yinc;

	cy = 0;
	fy = VSIZE - 1;
	
	p0x = mLevel->player->x;
	p0y = mLevel->player->y;
	p1x = ds0 * cosp - sinp * w2 + p0x;
	p1y = ds0 * sinp + cosp * w2 + p0y;
	xinc = w640 * sinp;
	yinc = -w640 * cosp;
	
	for(x=0; x<HSIZE; x++) 
	{
		px = p1x - p0x;
		py = p1y - p0y;
		
		if(abs(px) > abs(py))
		{ 
			usepx = 1; 
			ds = hw * px; 
		}
		else
		{ 
			usepx = 0; 
			ds = hw * py; 
		}

		drawWallSlice(mLevel->player, x, -1, 0, VSIZE - 1);
		
		p1x += xinc;
		p1y += yinc;		
	}
	int k;

	for(k=fy+1; k<VSIZE; k++)
	{
		if(k < 0) continue;
		if(fc[k].x == -1) continue;

		getuvfloor(k, fc[k].sector, mLevel->player, px, py, 1);
		textureFloorCeiling(fc[k].x, x, k, fc[k].sector, fc[k].sector->floortex, fc[k].u, fc[k].v, g_u, g_v);
		fc[k].x = -1;
	}

	for(k=cy-1; k>=0; k--)
	{
		if(k > VSIZE - 1) continue;
		if(fc[k].x == -1) continue;

		getuvceil(k, fc[k].sector, mLevel->player, px, py, 1);
		textureFloorCeiling(fc[k].x, x, k, fc[k].sector, fc[k].sector->ceiltex, fc[k].u, fc[k].v, g_u, g_v);
		fc[k].x = -1;
	}

	if(globalflag < 65536) globalflag++;
	else globalflag = 0;
}	

void Renderer::drawScreen()
{
	sinp = sin(mLevel->player->angle);
	cosp = cos(mLevel->player->angle);
	drawWalls();
}

Renderer::Renderer(GraphicsContext *context, Level *level)
{
	mContext = context;
	mLevel = level;

	int i;
	for(i=0;i<VSIZE;i++) fc[i].x=-1;
}

Renderer::~Renderer()
{
}