#include "Texture.h"
#include "Debug.h"

Texture LoadBMP(char *filename, int width, int height)
{
	HANDLE	BitmapFile;
	DWORD	dummy;
	int 	y, x;
	UCHAR 	tempdata[1200];
	Texture tex;

	tex.width = width;
	tex.height = height;
	BitmapFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(BitmapFile == INVALID_HANDLE_VALUE) DebugString("Could not open texture file\n");
	tex.data = new UCHAR[width * height * 3];
	DebugString("Reading in file...\n");
	for(y=0; y<height; y++)
	{
		for(x=0; x<3; x++)
			{
				ReadFile(BitmapFile, tempdata, width, &dummy, NULL);
				memcpy(tex.data + 3 * y * width + width * x, tempdata, width);
			}
	}	
	CloseHandle(BitmapFile);
	return tex;
}
