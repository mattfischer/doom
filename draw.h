#include "global.h"

void DrawScreen();
void LockBack();
void UnlockBack();
void ClearBack();
void vline(int,int,int,DWORD);
void hline(int,int,int,int,int,int);
void DrawWallSlice(int,struct Sector*,int,int,int);
void Drawfclice(int,struct Sector*,int,int);
void DrawCeilingSlice(int,struct Sector*,int,int);
void Drawfc();
void DrawCeilings();
void DrawWalls();
void texwall(int,int,int,int,int,int,int,int,int);
void texfloorceil(int,int,int,double,double,double,double);
void getuvfloor(int,struct Sector*,double,double,int);
void getuvceil(int,struct Sector*,double,double,int);

void DrawOverhead();
void line(int,int,int,int,DWORD);
void circle(int,int,int,DWORD);
void vertex(int,int);
void PutPixel(int,int,DWORD);

UCHAR *vidmem=NULL;
DWORD pitch=0;
int horizon=VSIZE/2;
extern struct Sector *sectors;
extern int numsectors;
extern struct Player player;
extern LPDIRECTDRAWSURFACE lpDDSBack;
extern LPDIRECTDRAWSURFACE lpDDSBack2;
extern struct Texture **textures;

extern void DebugString(char*);
extern void HandleBadResult(int,int);
extern void ClearSurface(LPDIRECTDRAWSURFACE);
extern double pxb,pyb,perxb,peryb,parxb,paryb;
extern int thesector,thewall;
extern int ubermode;

double us[HSIZE][VSIZE];
double vs[HSIZE][VSIZE];

int worldtotex=16;
int *activesectors;
int numactivesectors;

struct MapInfo mapinfo;

extern Point *point1;
extern Point *point2;
extern Point *point3;
extern Point *point4;
extern Point screenpoint;
extern bool pointselected;