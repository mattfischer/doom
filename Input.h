#ifndef INPUT_H
#define INPUT_H

#include <windows.h>
#include <math.h>

struct Player;
struct MapInfo;
void ProcessInput(Player *player, MapInfo *mapInfo, int *horizon);

#endif