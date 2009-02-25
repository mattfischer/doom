#include "global.h"
#include "ddraw.h"

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

GraphicsContext *SetupDirectDraw(HWND hWnd)
{
	GraphicsContext *context = new GraphicsContext;
	HRESULT ddrval;
	DDSURFACEDESC ddSD;
	DDSCAPS ddsCaps;

	ddrval = DirectDrawCreate(NULL, &context->lpDD, NULL);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 1);
		delete context;
		return NULL;
	}

	ddrval = context->lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 2);
		delete context;
		return NULL;
	}
	ddrval = context->lpDD->SetDisplayMode(HSIZE, VSIZE, 32);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 3);
		delete context;
		return NULL;
	}

	ZeroMemory((LPVOID)&ddSD, (DWORD)sizeof(ddSD));
	ddSD.dwSize = sizeof(ddSD);
	ddSD.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	ddSD.dwBackBufferCount = 2;


	ddrval = context->lpDD->CreateSurface(&ddSD, &context->lpDDSPrimary, NULL);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 4);
		delete context;
		return NULL;
	}
	ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	ddrval = context->lpDDSPrimary->GetAttachedSurface(&ddsCaps, &context->lpDDSBack);

	if(ddrval != DD_OK)
	{
		HandleBadResult(ddrval, 5);
		delete context;
		return NULL;
	}

	ddsCaps.dwCaps = DDSCAPS_FLIP;
	context->lpDDSBack->GetAttachedSurface(&ddsCaps, &context->lpDDSBack);

	return context;
}


void CleanupDirectDraw(GraphicsContext *context)
{
	if(context->lpDD)
	{
		if(context->lpDDSPrimary)
			{
				context->lpDDSPrimary->Release();
			}
		context->lpDD->Release();
	}

	delete context;
}

void ClearSurface(LPDIRECTDRAWSURFACE Surface)
{
	DDBLTFX Fx;
	Fx.dwSize=sizeof(DDBLTFX);

	Fx.dwFillColor=0;

	Surface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&Fx);

}


void FlipSurfaces(GraphicsContext *context)
{
	int ddrval;
	ddrval = context->lpDDSPrimary->Flip(NULL,0);
}

void RestoreStuff(GraphicsContext *context)
{
#ifdef NOVIS
	return;
#endif

 DisplayMode(context, 1);
 context->lpDDSPrimary->Restore();
 context->lpDDSBack->Restore();
}

void DisplayMode(GraphicsContext *context, int mode)
{
#ifdef NOVIS
	return;
#endif
	if(mode==0) context->lpDD->RestoreDisplayMode();
	if(mode==1) context->lpDD->SetDisplayMode(HSIZE,VSIZE,32);
}

void LockBack(GraphicsContext *context)
{
	DDSURFACEDESC ddsd;
	DWORD ddrval;
	ddsd.dwSize=sizeof(DDSURFACEDESC);
	
	ddrval = context->lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	if(ddrval != DD_OK) HandleBadResult(ddrval, 0);

	context->vidmem = (UCHAR*)ddsd.lpSurface;
	context->pitch = ddsd.lPitch;
}

void UnlockBack(GraphicsContext *context)
{
	context->lpDDSBack->Unlock(NULL);
	context->vidmem=NULL;
	context->pitch=0;
}

