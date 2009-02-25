#ifndef GLOBAL_H
#define GLOBAL_H

#include "macros.h"
#include <ddraw.h>
#include <stdlib.h>
#include <malloc.h>

//#define NOVIS

#define HSIZE 800
#define VSIZE 600
struct Sector;
struct Point {
	double x;
	double y;
};

struct Texture {
	int width;
	int height;
	UCHAR *data;
};

struct Sector;

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

struct Sector {
	int num;
	double floorheight;
	double ceilingheight;
	int numwalls;
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

struct Player {
	double x;
	double y;
	double height;
	double fromfloor;
	double angle;
	Sector *sector;
};

struct MapInfo {
	int show;
	int rotate;
	double zoom;
};

struct FloorCeilingInfo {
	int x;
	double u;
	double v;
	Sector *sector;
};


#define WALL_NORMAL 0
#define WALL_ADJOINED 1

#define PI 3.14159

#endif