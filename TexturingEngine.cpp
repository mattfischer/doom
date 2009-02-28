#include "TexturingEngine.h"

#include "Texture.h"
#include "GraphicsContext.h"

#define TEXTURINGENGINE TexturingEngineFixed

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
		u = (int)(u0 + x * (u1 - u0) / (x1 - x0));
		v = (int)(v0 + x * (v1 - v0) / (x1 - x0));

		if(u < 0) u += texture->width * (abs(u) / texture->width + 1);
		if(v < 0) v += texture->height * (abs(v) / texture->height + 1);
		u %= texture->width;
		v %= texture->height;

		DWORD *src = texture->data + v * texture->width + u;
		DWORD *dst = mContext->frameBuffer() + y * mContext->pitch() + x0 + x;

		*(dst) = *(src);
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

		DWORD *dst = mContext->frameBuffer() + (y + y0) * mContext->pitch() + x;
		DWORD *src = texture->data + ty * texture->width + tx;

		*dst = *src;
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

	DWORD *dst = mContext->frameBuffer() + y0 * mContext->pitch() + x;

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

		DWORD *src = texture->data + (int)ty * texture->width + tx;

		*dst = *src;
	
		ty += tyinc;
		if(ty > texture->height) ty -= texture->height * ((int)ty / texture->height);
		dst += mContext->pitch();
	}
}

void TexturingEngineFloat::textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1)
{
	int x;
	DWORD *src, *dst;

	if(x1 < x0) return;
	
	dst = mContext->frameBuffer() + y * mContext->pitch() + x0;

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
		src = texture->data + vi * texture->width + ui;

		*(dst) = *(src);
		
		dst++;

		u += uinc;
		v += vinc;
	}
}




class TexturingEngineFixed : public TexturingEngine
{
public:
	TexturingEngineFixed(GraphicsContext *context) : TexturingEngine(context) {}
	virtual void textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1);
	virtual void textureWall(int x,int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy);
};

void TexturingEngineFixed::textureWall(int x, int y0, int y1, Texture *texture, int tx, int ty0, int ty1, int miny, int maxy)
{
	int width = texture->width;
	int height = texture->height;

	DWORD *dst = mContext->frameBuffer() + y0 * mContext->pitch() + x;
	int pitch = mContext->pitch();

	int tyinc = ty1 - ty0 - 1;
	int ty = ty0;
	int error = 0;
	int div = y1 - y0;
	int starty = 0;

	int endy = y1 - y0;
	if(y0 + endy > maxy) endy = maxy - y0;
	
	tx %= texture->width;
	ty %= height;

	if(y0 < miny)
	{
		error += tyinc * (miny - y0);
		
		int delta = error / div;
		ty += delta;
		error -= delta * div;

		ty %= height;
		dst += pitch * (miny - y0);
		starty = miny - y0;
	}

	DWORD *data = texture->data;
	DWORD *src = data + ty * texture->width + tx;
	
	for(int y = starty; y <= endy; y++) 
	{
		*dst = *src;
	
		error += tyinc;
		if(error > y1 - y0) 
		{
			error -= y1 - y0;
			src += texture->width;

			if(src >= data + texture->width * texture->height)
			{
				src -= texture->width * texture->height;
			}
		}

		dst += pitch;
	}
}

void TexturingEngineFixed::textureFloorCeiling(int x0, int x1, int y, Texture *texture, double u0, double v0, double u1, double v1)
{
	int x;
	DWORD *src, *dst;
	int SHIFT = 16;

	if(x1 < x0) return;
	
	dst = mContext->frameBuffer() + y * mContext->pitch() + x0;

	double u = u0;
	double v = v0;
	if(u < 0) u += texture->width * ((int)abs(u) / texture->width + 1);
	if(v < 0) v += texture->height * ((int)abs(v) / texture->height + 1);
	if(u >= texture->width) u -= texture->width * ((int)u / texture->width);
	if(v >= texture->height) v -= texture->height * ((int)v / texture->height);

	int U = (int)(u * (1 << SHIFT));
	int V = (int)(v * (1 << SHIFT));

	double uinc = (u1 - u0) / (x1 - x0);
	int UINC = (int)(uinc * (1 << SHIFT));
	double vinc = (v1 - v0) / (x1 - x0);
	int VINC = (int)(vinc * (1 << SHIFT));

	int WIDTH = texture->width << SHIFT;
	int HEIGHT = texture->height << SHIFT;

	for(x=0; x <= x1-x0; x++)
	{
		if(U < 0) U += WIDTH;
		if(V < 0) V += HEIGHT;
		if(U > WIDTH) U -= WIDTH;
		if(V > HEIGHT) V -= HEIGHT;

		src = texture->data + (V >> SHIFT) * texture->width + (U >> SHIFT);

		*(dst) = *(src);
		
		dst++;

		U += UINC;
		V += VINC;
	}
}

TexturingEngine *TexturingEngine::createTexturingEngine(GraphicsContext *context)
{
	return new TEXTURINGENGINE(context);
}
