#include "GraphicsContext.h"

#include "global.h"

#include "Debug.h"

extern HANDLE DebugFile;

void HandleBadResult(int ddrval,int level)
{

	char buff[100];
	char buff2[100];
	wsprintf(buff2,"Level %i:",level);

	WriteFile(DebugFile,"Error:",6,(LPDWORD)buff,(LPOVERLAPPED)NULL);
	WriteFile(DebugFile,buff2,strlen(buff2),(LPDWORD)buff,(LPOVERLAPPED)NULL);

	switch(ddrval)
	{
		case DDERR_DCALREADYCREATED:
			WriteFile(DebugFile,"DC already created",18,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;
		case DDERR_CANTCREATEDC:
			WriteFile(DebugFile,"Cant make DC",12,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;
		case DDERR_DIRECTDRAWALREADYCREATED:
			WriteFile(DebugFile,"Already created",15,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;

		case DDERR_INVALIDCAPS:
			WriteFile(DebugFile,"Invalid caps",12,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;
		case DDERR_INVALIDPARAMS:
			WriteFile(DebugFile,"Invalid params",14,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;
		case DDERR_INVALIDRECT:
			WriteFile(DebugFile,"Invalid rect\n",12,(LPDWORD)buff,(LPOVERLAPPED)NULL);
			break;

	}
}

GraphicsContext::GraphicsContext(HWND hWnd, int width, int height)
{
	HRESULT ddrval;
	DDSURFACEDESC ddSD;
	DDSCAPS ddsCaps;

	mWidth = width;
	mHeight = height;

	ddrval = DirectDrawCreate(NULL, &mDirectDraw, NULL);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 1);
	}

	ddrval = mDirectDraw->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 2);
	}

	mDirectDraw->SetDisplayMode(mWidth, mHeight, 32);

	ZeroMemory((LPVOID)&ddSD, (DWORD)sizeof(ddSD));
	ddSD.dwSize = sizeof(ddSD);
	ddSD.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	ddSD.dwBackBufferCount = 2;

	ddrval = mDirectDraw->CreateSurface(&ddSD, &mPrimarySurface, NULL);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 4);
	}
	ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	ddrval = mPrimarySurface->GetAttachedSurface(&ddsCaps, &mBackSurface);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 5);
	}

	ddsCaps.dwCaps = DDSCAPS_FLIP;
	mBackSurface->GetAttachedSurface(&ddsCaps, &mBackSurface);

	mActive = true;
}


GraphicsContext::~GraphicsContext()
{
	setActive(false);

	if(mDirectDraw)
	{
		if(mPrimarySurface)
		{
			mPrimarySurface->Release();
		}

		mDirectDraw->Release();
	}
}

void ClearSurface(LPDIRECTDRAWSURFACE Surface)
{
	DDBLTFX Fx;
	Fx.dwSize=sizeof(DDBLTFX);

	Fx.dwFillColor=0;

	Surface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&Fx);
}


void GraphicsContext::flip()
{
	int ddrval;
	ddrval = mPrimarySurface->Flip(NULL,0);
}

void GraphicsContext::setActive(bool active)
{
	if(active)
	{
		mDirectDraw->SetDisplayMode(mWidth, mHeight, 32);
		mPrimarySurface->Restore();
		mBackSurface->Restore();
	}
	else
	{
		mDirectDraw->RestoreDisplayMode();
	}

	mActive = active;
}

bool GraphicsContext::active()
{
	return mActive;
}

void GraphicsContext::setLocked(bool locked)
{
	if(locked)
	{
		DDSURFACEDESC ddsd;
		DWORD ddrval;
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		
		ddrval = mBackSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		if(ddrval != DD_OK) HandleBadResult(ddrval, 0);

		mFrameBuffer = (UCHAR*)ddsd.lpSurface;
		mPitch = ddsd.lPitch;
	}
	else 
	{
		mBackSurface->Unlock(NULL);
		mFrameBuffer = NULL;
		mPitch = 0;
	}

	mLocked = locked;
}

bool GraphicsContext::locked()
{
	return mLocked;
}

UCHAR *GraphicsContext::frameBuffer()
{
	return mFrameBuffer;
}

int GraphicsContext::pitch()
{
	return mPitch;
}

int GraphicsContext::width()
{
	return mWidth;
}

int GraphicsContext::height()
{
	return mHeight;
}