#ifndef DRAW_H
#define DRAW_H

struct Level;
class GraphicsContext;
struct MapInfo;

void DrawScreen(GraphicsContext *context, Level *level);
void DrawOverhead(GraphicsContext *context, MapInfo *mapInfo, Level *level);
void InitWalls();

#endif