#include "GraphicsContext.h"

#include "Debug.h"

GraphicsContext::GraphicsContext(HWND hWnd, int width, int height)
{
	mD3d = Direct3DCreate9(D3D_SDK_VERSION);

	mD3dDevice = NULL;
	mHWnd = hWnd;
	mWidth = width;
	mHeight = height;

	setActive(true);
}

GraphicsContext::~GraphicsContext()
{
	setActive(false);

	if(mD3d)
	{
		if(mD3dDevice)
		{
			mD3dDevice->Release();
		}

		mD3d->Release();
	}
}

void GraphicsContext::flip()
{
	int ddrval;
	ddrval = mD3dDevice->Present(NULL, NULL, NULL, NULL);
}

void GraphicsContext::setActive(bool active)
{
	if(active)
	{
		D3DPRESENT_PARAMETERS d3dpp;

		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = FALSE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferWidth = mWidth;
		d3dpp.BackBufferHeight = mHeight;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		if(mD3dDevice == NULL)
		{
			mD3d->CreateDevice(D3DADAPTER_DEFAULT,
							   D3DDEVTYPE_HAL,
							   mHWnd,
							   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							   &d3dpp,
							   &mD3dDevice);
		}
		else
		{
			mD3dDevice->Reset(&d3dpp);
		}
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
		HRESULT result;
		D3DLOCKED_RECT rect;

		result = mD3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &mD3dSurface);
		result = mD3dSurface->LockRect(&rect, NULL, 0);

		mFrameBuffer = (DWORD*)rect.pBits;
		mPitch = rect.Pitch / sizeof(DWORD);
	}
	else 
	{
		mD3dSurface->UnlockRect();
		mD3dSurface->Release();
		mD3dSurface = NULL;
		mFrameBuffer = NULL;
		mPitch = 0;
	}

	mLocked = locked;
}

bool GraphicsContext::locked()
{
	return mLocked;
}

DWORD *GraphicsContext::frameBuffer()
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