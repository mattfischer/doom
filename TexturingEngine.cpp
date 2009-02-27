#include "TexturingEngine.h"

#include "Texture.h"
#include "GraphicsContext.h"

TexturingEngine::TexturingEngine(GraphicsContext *context)
{
	mContext = context;
}

class TexturingEngineReference : public TexturingEngine
{
public:
	TexturingEngineReference(GraphicsContext *context) : TexturingEngine(context) {}
	virtual void textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1);
	virtual void textureWall(int x,int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy);
};

void TexturingEngineReference::textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1)
{
	int u, v;
	int x;

	if(x1 < x0) return;

	for(x=0; x <= x1-x0; x++)
	{
		u = u0 + x * (u1 - u0) / (x1 - x0);
		v = v0 + x * (v1 - v0) / (x1 - x0);

		if(u < 0) u += texture->width * (abs(u) / texture->width + 1);
		if(v < 0) v += texture->height * (abs(v) / texture->height + 1);
		u %= texture->width;
		v %= texture->height;

		UCHAR *src = texture->data + v * texture->width * 3 + u * 3;
		UCHAR *dst = mContext->frameBuffer() + y * mContext->pitch() + (x0 + x) * 4;

		*(dst) = *(src);
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
}

void TexturingEngineReference::textureWall(int x, int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy)
{
	if(tx < 0) tx += texture->width * (abs(tx) / texture->width + 1);
	tx %= texture->width;

	for(int y = 0; y <= y1 - y0; y++) 
	{
		if(y + y0 < miny) continue;
		if(y + y0 > maxy) continue;

		int ty = ty0 + y * (ty1 - ty0 - 1) / (y1 - y0);
		if(ty < 0) ty += texture->height * (abs(ty)/ texture->height + 1);
		ty %= texture->height;

		UCHAR *dst = mContext->frameBuffer() + (y + y0) * mContext->pitch() + x * 4;
		UCHAR *src = texture->data + ty * texture->width * 3 + tx * 3;

		*dst = *src;
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	}
}




class TexturingEngineFloat : public TexturingEngine
{
public:
	TexturingEngineFloat(GraphicsContext *context) : TexturingEngine(context) {}
	virtual void textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1);
	virtual void textureWall(int x,int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy);
};

void TexturingEngineFloat::textureWall(int x, int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy)
{
	if(tx < 0) tx += texture->width * (abs(tx) / texture->width + 1);
	tx %= texture->width;
	double tyinc = (double)(ty1 - ty0 - 1) / (y1 - y0);
	double ty = ty0;
	if(ty < 0) ty += texture->height * (abs(ty)/ texture->height + 1);

	UCHAR *dst = mContext->frameBuffer() + y0 * mContext->pitch() + x * 4;

	int starty = 0;

	if(y0 < miny)
	{
		ty += tyinc * (miny - y0);
		if(ty > texture->height) ty -= texture->height * ((int)ty / texture->height);
		dst += mContext->pitch() * (miny - y0);
		starty = miny - y0;
	}

	for(int y = starty; y <= y1 - y0; y++) 
	{
		if(y + y0 > maxy) break;

		UCHAR *src = texture->data + (int)ty * texture->width * 3 + tx * 3;

		*dst = *src;
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
	
		ty += tyinc;
		if(ty > texture->height) ty -= texture->height * ((int)ty / texture->height);
		dst += mContext->pitch();
	}
}

void TexturingEngineFloat::textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1)
{
	int x;
	UCHAR *src, *dst;
	int width3;

	if(x1 < x0) return;
	
	width3 = texture->width * 3;
	dst = mContext->frameBuffer() + y * mContext->pitch() + x0 * 4;

	double u = u0;
	double v = v0;
	
	double uinc = (u1 - u0) / (x1 - x0);
	double vinc = (v1 - v0) / (x1 - x0);

	for(x=0; x <= x1-x0; x++)
	{
		if(u < 0) u += texture->width * ((int)abs(u) / texture->width + 1);
		if(v < 0) v += texture->height * ((int)abs(v) / texture->height + 1);
		if(u >= texture->width) u -= texture->width * ((int)u / texture->width);
		if(v >= texture->height) v -= texture->height * ((int)v / texture->height);

		int ui = (int)u;
		int vi = (int)v;
		src = texture->data + vi * width3 + ui * 3;

		*(dst) = *(src);
		*(dst + 1) = *(src + 1);
		*(dst + 2) = *(src + 2);
		
		dst += 4;

		u += uinc;
		v += vinc;
	}
}

TexturingEngine *TexturingEngine::createTexturingEngine(GraphicsContext *context)
{
	return new TexturingEngineFloat(context);
}
