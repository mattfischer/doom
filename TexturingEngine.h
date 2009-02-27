#ifndef TEXTURER_H
#define TEXTURER_H

class GraphicsContext;
struct Texture;

class TexturingEngine
{
public:
	static TexturingEngine *createTexturingEngine(GraphicsContext *context);

	virtual void textureWall(int x, int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy) = 0;
	virtual void textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1) = 0;

protected:
	TexturingEngine(GraphicsContext *context);

	GraphicsContext *mContext;
};

#endif