#ifndef DDRAW_H
#define DDRAW_H

#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>

class GraphicsContext {
public:
	GraphicsContext(HWND hWnd);
	~GraphicsContext();

	void setLocked(bool locked);
	bool locked();

	UCHAR *frameBuffer();
	int pitch();

	void flip();

	void setActive(bool active);
	bool active();

protected:
	LPDIRECTDRAW mDirectDraw;
	LPDIRECTDRAWSURFACE mPrimarySurface;
	LPDIRECTDRAWSURFACE mBackSurface;
	UCHAR *mFrameBuffer;
	int mPitch;
	bool mActive;
	bool mLocked;
};


#endif