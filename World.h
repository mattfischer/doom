#ifndef WORLD_H
#define WORLD_H

#include <windows.h>

#define WORLDTOTEX 16

struct Wall;
struct Texture;

#define WALL_NORMAL 0
#define WALL_ADJOINED 1

struct Sector {
	int num;
	double floorheight;
	double ceilingheight;
	int numWalls;
	int lastwall;
	int beenin;
	Texture *ceiltex;
	Texture *floortex;
	struct Wall *walls;

	int fy;
	int fy2;

	int cy;
	int cy2;
};

struct Point {
	double x;
	double y;
};

struct Wall {
	struct Point start;
	struct Point end;
	int flags;
	Sector *adjoin;
	int mirror;
	double length;
	int tophoriz;
	int midhoriz;
	int botthoriz;
	int topoffset;
	int midoffset;
	int bottomoffset;
	int topbott;
	int midbott;
	int bottombott;
	int topmod;
	int midmod;
	int bottmod;
	Texture *toptex;
	Texture *midtex;
	Texture *botttex;
	double soffset;
	DWORD topcol;
	DWORD midcol;
	DWORD bottcol;
};

struct Player {
	double x;
	double y;
	double height;
	double fromfloor;
	double angle;
	Sector *sector;
};

#endif