#include "Draw.h"

#include "GraphicsContext.h"

void PutPixel(GraphicsContext *context, int x, int y, DWORD color)
{
	if(x<0 || x>context->width()-1 || y<0 || y>context->height()-1) return;

	UCHAR *frameBuffer = context->frameBuffer();
	int pitch = context->pitch();

	*(frameBuffer + y * pitch + x * 4) = color&0xFF;
	*(frameBuffer + y * pitch + x * 4 + 1) = (color>>8)&0xFF;
	*(frameBuffer + y * pitch + x * 4 + 2) = (color>>16)&0xFF;
}

void vline(GraphicsContext *context, int x, int y0, int y1, DWORD color)
{
	int y;
	DWORD val;
	UCHAR *frameBuffer = context->frameBuffer();
	int pitch = context->pitch();

	val = y0 * pitch;
	for(y=y0; y<=y1; y++)
	{
		*(frameBuffer + val + 4 * x) = (UCHAR)color&0xFF;
		*(frameBuffer + val + 4 * x + 1) = (UCHAR)(color>>8)&0xFF;
		*(frameBuffer + val + 4 * x + 2) = (UCHAR)(color>>16)&0xFF;
		val += pitch;
	}
}

static void hline(GraphicsContext *context, int x0, int x1, int y, DWORD color)
{
	int x;
	DWORD val;
		
#ifdef NOVIS
	return;
#endif

	val = y * context->pitch();
	for(x=x0; x<=x1; x++)
	{
		*(context->frameBuffer() + val + x * 4) = (UCHAR)color&0xFF;
		*(context->frameBuffer() + val + x * 4 + 1) = (UCHAR)(color>>8)&0xFF;
		*(context->frameBuffer() + val + x * 4 + 2) = (UCHAR)(color>>16)&0xFF;
	}

}

void DrawLine(GraphicsContext *context, int x0, int y0, int x1, int y1, DWORD color)
{
	int xd,yd;
	int error;
	int x,y;
	int xi1,xi2,yi1,yi2;
	int x0c,y0c,x1c,y1c;
	int e1,e2;
	int i,i2;
	
	if(y0 == y1)
	{
		hline(context, x0, x1, y0, color);
		return;
	}

	if(x0 == x1)
	{
		vline(context, x0, y0, y1, color);
		return;
	}

	xd = abs(x1 - x0);
	yd = abs(y1 - y0);
	x = x0;
	y = y0;
	
	if(x1 > x0) xi1 = 1; else xi1 = -1;
	if(y1 > y0) yi1 = 1; else yi1 = -1;
	
	i = 0;
	if(xd > yd) 
	{ 
		i2 = xd;
		error = xd / 2;
		e1 = yd;
		e2 = xd;
		xi2 = xi1; 
		yi2 = 0;
	} 
	else
	{
		i2 = yd;
		error = yd/2;
		e1 = xd;
		e2 = yd;
		xi2 = 0;
		yi2 = yi1;
	}

	while(i != i2)
	{
		PutPixel(context, x, y, color);
		error += e1;
		if(error > e2)
		{
			error -= e2;
			x += xi1;
			y += yi1;
		}
		else 
		{
			x += xi2;
			y += yi2;
		}
		i++;
	}
}

void DrawCircle(GraphicsContext *context, int cx, int cy, int rad, DWORD color)
{
	int d;
	int x,y;

	d = 3 - (2 * rad);
	x = 0;
	y = rad;

	while(1)
	{
		PutPixel(context, cx + x, cy + y, color);
		PutPixel(context, cx - x, cy + y, color);
		PutPixel(context, cx + x, cy - y, color);
		PutPixel(context, cx - x, cy - y, color);
		PutPixel(context, cx + y, cy + x, color);
		PutPixel(context, cx + y, cy - x, color);
		PutPixel(context, cx - y, cy + x, color);
		PutPixel(context, cx - y, cy - x, color);

		if(x==y || x==rad) break;
		if(d < 0) d += 4 * x + 6;
		else
		{
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}
