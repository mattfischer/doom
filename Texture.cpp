#include "Texture.h"
#include "Debug.h"

#include <windows.h>

Texture LoadBMP(char *filename)
{
	HANDLE	file;
	DWORD	dummy;
	Texture tex;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(file == INVALID_HANDLE_VALUE) DebugString("Could not open texture file\n");

	ReadFile(file, &bmfh, sizeof(bmfh), &dummy, NULL);
	ReadFile(file, &bmih, sizeof(bmih), &dummy, NULL);

	tex.width = bmih.biWidth;
	tex.height = bmih.biHeight;
	tex.data = new DWORD[tex.width * tex.height];
	for(int y = tex.height - 1; y>=0; y--)
	{
		ReadFile(file, tex.data + y * tex.width, sizeof(DWORD) * tex.width, &dummy, NULL);
	}
	CloseHandle(file);

	return tex;
}