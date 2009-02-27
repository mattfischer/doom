#ifndef DRAW_H
#define DRAW_H

#include <windows.h>

class GraphicsContext;

void PutPixel( GraphicsContext *context, int x, int y, DWORD color);
void DrawLine(GraphicsContext *context, int x0, int y0, int x1, int y1, DWORD color);
void DrawCircle(GraphicsContext *context, int cx, int cy, int rad, DWORD color);

#endif