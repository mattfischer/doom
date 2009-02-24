#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>

int SetupDirectDraw();
void CleanupDirectDraw();
void HandleBadResult(int,int);
void ClearSurface(LPDIRECTDRAWSURFACE);
void FlipSurfaces();
void DisplayMode(int);
void RestoreStuff();
struct Texture *LoadBMP(char*,int,int);

extern void DebugString(char*);
extern HWND hMainWindow;
extern HANDLE DebugFile;

extern int surf;
LPDIRECTDRAW lpDD;
LPDIRECTDRAWSURFACE lpDDSPrimary;
LPDIRECTDRAWSURFACE lpDDSBack;
LPDIRECTDRAWSURFACE lpDDSBack2;
LPDIRECTDRAWCLIPPER lpDDClipper;

