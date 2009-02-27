#ifndef DRAW_H
#define DRAW_H

struct Level;
class GraphicsContext;
struct MapInfo;
struct Player;
struct Texture;

class Renderer
{
public:
	Renderer(GraphicsContext *context, Level *level);
	~Renderer();

	void drawScreen();
	void drawMap(MapInfo *mapInfo);

	void setHorizon(int horizon);
	int horizon();

protected:
	void drawWalls();
	void drawWallSlice(Player *player, int x, int ignore, int miny, int maxy);

	void textureFloorCeiling(int x0, int x1, int y, struct Sector *sector, Texture *texture, double u0, double v0, double u1, double v1);
	void textureWall(int x,int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy, int fade);

	void getCeilingUV(int y, Sector *sector, Player *player, double data1, double data2, int datatype);
	void getFloorUV(int y, Sector *sector, Player *player, double data1, double data2, int datatype);

	GraphicsContext *mContext;
	Level *mLevel;

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