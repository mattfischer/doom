#ifndef DDRAW_H
#define DDRAW_H

#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>

struct GraphicsContext {
	LPDIRECTDRAW lpDD;
	LPDIRECTDRAWSURFACE lpDDSPrimary;
	LPDIRECTDRAWSURFACE lpDDSBack;
	UCHAR *vidmem;
	int pitch;
};

GraphicsContext *SetupDirectDraw(HWND);
void CleanupDirectDraw(GraphicsContext *);
void FlipSurfaces(GraphicsContext *);
void DisplayMode(GraphicsContext *, int);
void RestoreStuff(GraphicsContext *);
void LockBack(GraphicsContext *);
void UnlockBack(GraphicsContext *);

#endif