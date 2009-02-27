#ifndef LEVEL_H
#define LEVEL_H

struct Texture;
struct Sector;

struct Player {
	double x;
	double y;
	double height;
	double fromfloor;
	double angle;
	Sector *sector;
};

struct Level {
	Texture *textures;
	int numTextures;

	Sector *sectors;
	int numSectors;

	Player *player;
};

Level *LoadLevel();

struct MapInfo {
	int show;
	int rotate;
	double zoom;
};

#endif