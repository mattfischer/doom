#ifndef DRAW_H
#define DRAW_H

struct Level;
struct GraphicsContext;

void DrawScreen(Level *level, GraphicsContext *context);
void InitWalls();

#endif