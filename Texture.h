#ifndef TEXTURE_H
#define TEXTURE_H

#include <windows.h>

struct Texture {
	int width;
	int height;
	UCHAR *data;
};

Texture LoadBMP(char*, int, int);

#endif