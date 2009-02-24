void LoadLevel();
void DoWallStuff();

struct Sector *sectors;
struct Player player;

extern void DebugString(char*);
extern struct Texture *LoadBMP(char*,int,int);
extern int worldtotex;
extern int *activesectors;

extern struct MapInfo mapinfo;

struct Texture **textures;
int numtextures;
int numsectors;

int ubermode=1;