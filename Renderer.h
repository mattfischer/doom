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

	void getCeilingUV(int y, Sector *sector, Player *player, double data1, double data2, int datatype);
	void getFloorUV(int y, Sector *sector, Player *player, double data1, double data2, int datatype);

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
};

#endif