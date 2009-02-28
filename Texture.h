#ifndef TEXTURE_H
#define TEXTURE_H

#include <windows.h>

struct Texture {
	int width;
	int height;
	DWORD *data;
};

Texture LoadBMP(char *filename);

#endif