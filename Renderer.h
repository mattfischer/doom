#ifndef RENDERER_H
#define RENDERER_H

struct Level;
class GraphicsContext;
class TexturingEngine;
struct MapInfo;
struct Player;
struct Texture;
struct Point;
struct Sector;

class Renderer
{
public:
	Renderer(GraphicsContext *context, Level *level);
	~Renderer();

	void drawScreen(Player *player);
	void drawMap(Player *player, int rotate, double zoom, Point *selectedPoint);

	void setHorizon(int horizon);
	int horizon();

protected:
	void drawWalls(Player *player);
	void drawWallSlice(Player *player, int x, int ignore, int miny, int maxy);

	void getCeilingUV(int y, Sector *sector, Player *player, double px, double py, double *u, double *v);
	void getFloorUV(int y, Sector *sector, Player *player, double px, double py, double *u, double *v);

	GraphicsContext *mContext;
	Level *mLevel;

	TexturingEngine *mTexturingEngine;

	struct FloorCeilingInfo {
		int x;
		double u;
		double v;
		Sector *sector;
	};

	FloorCeilingInfo *mFloorCeilingInfo;

	int mHorizon;

	double px, py;
	double p0x, p0y;
	double p1x, p1y;
	double ds;
	double sinp, cosp;	
	int fy, cy;

	bool mUsePx;
	int mGlobalFlag;
};

#endif