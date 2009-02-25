#ifndef EDIT_H
#define EDIT_H

struct Level;
struct Player;
void SelectPoint(Level *,int,int);
void MovePoints(Player *player, int x, int y);

#endif