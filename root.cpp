#include "global.h"
#include "root.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_MOUSEMOVE:
			if(pointselected && wParam&MK_LBUTTON)
				MovePoints(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			SelectPoint(LOWORD(lParam),HIWORD(lParam));
			return 0;
		case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

		case WM_ACTIVATEAPP:
			if(started==0) return 0;
			if(wParam==TRUE)
			{
				ShowWindow(hMainWindow,SW_RESTORE);
				RestoreStuff();
				rungame=1;
			}
			if(wParam==FALSE)
			{
				ShowWindow(hMainWindow,SW_MINIMIZE);
				rungame=0;
				DisplayMode(0);
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
	
	DebugFile=CreateFile("debug.log",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	
	
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
			if(rungame) Game_Main();
	
	}


	Game_Shutdown();
	ShowCursor(TRUE);

	return (Msg.wParam);



}



