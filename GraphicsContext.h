#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include <windows.h>
#include <d3d9.h>
#include <stdlib.h>

class GraphicsContext {
public:
	GraphicsContext(HWND hWnd, int width, int height);
	~GraphicsContext();

	void setLocked(bool locked);
	bool locked();

	DWORD *frameBuffer();
	int pitch();

	void flip();

	void setActive(bool active);
	bool active();

	int width();
	int height();

protected:
	LPDIRECT3D9  		mD3d;
	LPDIRECT3DDEVICE9	mD3dDevice;
	LPDIRECT3DSURFACE9  mD3dSurface;

	HWND mHWnd;
	DWORD *mFrameBuffer;
	int mPitch;
	bool mActive;
	bool mLocked;
	int mWidth;
	int mHeight;
};


#endif