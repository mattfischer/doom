#include "global.h"
#include "root.h"
#include "Level.h"
#include "Edit.h"
#include "Debug.h"
#include "DDraw.h"
#include "Game.h"

extern Level *level;
GraphicsContext *context;

HWND hMainWindow;
int rungame;
int started;
extern HANDLE DebugFile;
extern LONG timer;
extern int frames;
extern bool pointselected;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_MOUSEMOVE:
			if(pointselected && wParam&MK_LBUTTON)
				MovePoints(level->player, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			SelectPoint(level, LOWORD(lParam),HIWORD(lParam));
			return 0;
		case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

		case WM_ACTIVATEAPP:
			if(started==0) return 0;
			if(wParam==TRUE)
			{
				ShowWindow(hMainWindow,SW_RESTORE);
				RestoreStuff(context);
				rungame=1;
			}
			if(wParam==FALSE)
			{
				ShowWindow(hMainWindow,SW_MINIMIZE);
				rungame=0;
				DisplayMode(context, 0);
			}
			return 0;

	}

	return DefWindowProc(hWnd,iMsg,wParam,lParam);

}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX WndClass;
	MSG Msg;
	char* szAppName="Doom thing";
	HINSTANCE hInstance;
	
	DebugInit();
		
	
	hInstance=hInst;
	started=0;



	WndClass.cbSize=sizeof(WNDCLASSEX);

	WndClass.style=(UINT)NULL;
	WndClass.lpfnWndProc=WndProc;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hInstance=hInstance;
	WndClass.hIcon=LoadIcon(0,IDI_APPLICATION);
	WndClass.hCursor=LoadCursor(0,IDC_ARROW);
	WndClass.hbrBackground=(HBRUSH)GetStockObject(NULL_BRUSH);
	WndClass.lpszMenuName=NULL;
	WndClass.lpszClassName=szAppName;
	WndClass.hIconSm=NULL;

	RegisterClassEx(&WndClass);

	hMainWindow=CreateWindowEx(
		(DWORD)NULL,
		szAppName,
		"Doom Thing",
		WS_VISIBLE | WS_POPUP,
	    0,
	    0,
		HSIZE,
		VSIZE,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		0
	);

	ShowWindow(hMainWindow, iCmdShow);

	UpdateWindow(hMainWindow);

	ShowCursor(TRUE);
	rungame=1;
	context = SetupDirectDraw(hMainWindow);

	if(Game_Init()) rungame=0;
	
	started=1;
	while(1)
		{
			
			if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				}
			if(Msg.message==WM_QUIT) break;
			if(rungame) Game_Main(context);
	
	}


	Game_Shutdown(context);
	ShowCursor(TRUE);

	return (Msg.wParam);



}



