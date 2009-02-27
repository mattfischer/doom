#ifndef EDIT_H
#define EDIT_H

struct Level;
struct Player;
struct MapInfo;
void SelectPoint(Level *, MapInfo *,int,int);
void MovePoints(Player *player, MapInfo *, int x, int y);

#endif