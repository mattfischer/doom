#include "global.h"

void SelectPoints(int,int);

Point *point1=NULL;
Point *point2=NULL;
Point *point3=NULL;
Point *point4=NULL;
Point screenpoint;
bool pointselected;

extern MapInfo mapinfo;

extern struct Sector *sectors;
extern int numsectors;
extern struct Player player;