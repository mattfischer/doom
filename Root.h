#include <windows.h>
#define WIN32_LEAN_AND_MEAN

extern int Game_Init();
extern void Game_Main();
extern void Game_Shutdown();
extern void DebugString(char*);
extern void RestoreStuff();
extern void DisplayMode(int);
extern void SelectPoint(int,int);
extern void MovePoints(int, int);

HWND hMainWindow;
int rungame;
int started;

extern HANDLE DebugFile;
extern LONG timer;
extern int frames;
extern bool pointselected;
