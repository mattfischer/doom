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

	/*
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

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	UCHAR *data = new UCHAR[width*height*4];
	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++) 
		{
			data[(height - y - 1)*width*4+x*4] = tex.data[y*width*3+x*3];
			data[(height - y - 1)*width*4+x*4+1] = tex.data[y*width*3+x*3+1];
			data[(height - y - 1)*width*4+x*4+2] = tex.data[y*width*3+x*3+2];
			data[(height - y - 1)*width*4+x*4+3] = 0;
		}
	}
	bmfh.bfType = 0x4D42;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width*height*4;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	char outfilename[100];
	strcpy(outfilename, filename);
	strcat(outfilename, ".bmp");
	HANDLE output = CreateFile(outfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(output, &bmfh, sizeof(bmfh), &dummy, NULL);
	WriteFile(output, &bmih, sizeof(bmih), &dummy, NULL);
	WriteFile(output, data, width*height*4, &dummy, NULL);
	CloseHandle(output);
	delete[] data;

	return tex;
}
*/