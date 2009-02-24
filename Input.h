#include <windows.h>
#include <math.h>

extern void DebugString(char*);
extern void DoWallCollisions(double,double);

extern struct Player player;
extern unsigned long frametimer;
extern int horizon;
extern int showmap;
extern struct MapInfo mapinfo;

void ProcessInput();

