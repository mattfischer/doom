#ifndef DEBUG_H
#define DEBUG_H

#include <windows.h>

void DebugInit();
void DebugShutdown();
void DebugString(char*);

void WriteStruct(void*, int);

#endif