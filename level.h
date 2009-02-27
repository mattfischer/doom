#ifndef LEVEL_H
#define LEVEL_H

#include "World.h"

struct Level {
	Texture *textures;
	int numTextures;

	Sector *sectors;
	int numSectors;

	Player playerStart;
};

Level *LoadLevel();

#endif