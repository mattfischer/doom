#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>

class GraphicsContext {
public:
	GraphicsContext(HWND hWnd, int width, int height);
	~GraphicsContext();

	void setLocked(bool locked);
	bool locked();

	UCHAR *frameBuffer();
	int pitch();

	void flip();

	void setActive(bool active);
	bool active();

	int width();
	int height();

protected:
	LPDIRECTDRAW mDirectDraw;
	LPDIRECTDRAWSURFACE mPrimarySurface;
	LPDIRECTDRAWSURFACE mBackSurface;
	UCHAR *mFrameBuffer;
	int mPitch;
	bool mActive;
	bool mLocked;
	int mWidth;
	int mHeight;
};


#endif