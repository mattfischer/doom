#include <windows.h>
#include <stdlib.h>
#include <ddraw.h>

int Game_Init();
void Game_Main();
void Game_Shutdown();

extern int SetupDirectDraw();

extern void DebugString(char*);
extern void ProcessInput();
extern void FlipSurfaces();
extern void CleanupDirectDraw();
extern void DisplayMode(int);
extern void LoadLevel();
extern void DrawScreen();
extern void LockBack();
extern void UnlockBack();
extern void InitWalls();

unsigned long frametimer;
DWORD dummy;
unsigned long timer;
extern HANDLE DebugFile;
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWSURFACE lpDDSBack;

