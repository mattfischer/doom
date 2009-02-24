#include "global.h"
#include "ddraw.h"

int SetupDirectDraw()
 {


	HRESULT ddrval;
	DDSURFACEDESC ddSD;
	DDSCAPS ddsCaps;

	ddrval=DirectDrawCreate(NULL,&lpDD,NULL);

	if(ddrval!=DD_OK)
		{
			HandleBadResult(ddrval,1);
			return 0;
		}

	ddrval=lpDD->SetCooperativeLevel(hMainWindow,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES);

	if(ddrval!=DD_OK)
		{
			HandleBadResult(ddrval,2);
			return 0;
		}
	ddrval=lpDD->SetDisplayMode(HSIZE,VSIZE,32);

	if(ddrval!=DD_OK)
		{
				HandleBadResult(ddrval,3);
				return 0;
		}
	ZeroMemory((LPVOID)&ddSD,(DWORD)sizeof(ddSD));
	ddSD.dwSize=sizeof(ddSD);
	ddSD.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddSD.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	ddSD.dwBackBufferCount=2;


	ddrval=lpDD->CreateSurface(&ddSD,&lpDDSPrimary,NULL);

	if(ddrval!=DD_OK)
		{
				HandleBadResult(ddrval,4);
				return 0;
		}
	ddsCaps.dwCaps=DDSCAPS_BACKBUFFER;

	ddrval=lpDDSPrimary->GetAttachedSurface(&ddsCaps,&lpDDSBack);

	if(ddrval!=DD_OK)
		{

			HandleBadResult(ddrval,5);
			return 0;
		}
	ddsCaps.dwCaps=DDSCAPS_FLIP;
	lpDDSBack->GetAttachedSurface(&ddsCaps,&lpDDSBack);

	ClearSurface(lpDDSBack);
	FlipSurfaces();
	ClearSurface(lpDDSBack);

	return 1;

}


void CleanupDirectDraw()
{
	if(lpDD!=NULL)
		{
			if(lpDDSPrimary!=NULL)
				{
					lpDDSPrimary->Release();
					lpDDSPrimary=NULL;
				}
			lpDD->Release();
			lpDD=NULL;
		}
}

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


void ClearSurface(LPDIRECTDRAWSURFACE Surface)
{
	DDBLTFX Fx;
	Fx.dwSize=sizeof(DDBLTFX);

	Fx.dwFillColor=0;

	Surface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&Fx);

}


void FlipSurfaces()
{
	int ddrval;
	ddrval=lpDDSPrimary->Flip(NULL,0);
}



void RestoreStuff()
{
#ifdef NOVIS
	return;
#endif

 DisplayMode(1);
 lpDDSPrimary->Restore();
 lpDDSBack->Restore();
}

void DisplayMode(int mode)
{
#ifdef NOVIS
	return;
#endif
	if(mode==0) lpDD->RestoreDisplayMode();
	if(mode==1) lpDD->SetDisplayMode(HSIZE,VSIZE,32);
}

struct Texture *LoadBMP(char *filename,int width,int height)
{
	HANDLE BitmapFile;
	DWORD			 dummy;
	int 			 y,x;
	UCHAR 			 tempdata[1200];
	struct Texture *tex;

	tex=(struct Texture*)malloc(sizeof(struct Texture));
	tex->width=width;
	tex->height=height;
	BitmapFile=CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if(BitmapFile==INVALID_HANDLE_VALUE) DebugString("Could not open texture file\n");
	tex->data=(UCHAR*)malloc(width*height*3);
	DebugString("Reading in file...\n");
	for(y=0;y<height;y++)
		{
			for(x=0;x<3;x++)
				{
					ReadFile(BitmapFile,tempdata,width,&dummy,NULL);
					memcpy(tex->data+3*y*width+width*x,tempdata,width);
				}
	}	
	CloseHandle(BitmapFile);
	return tex;
}
