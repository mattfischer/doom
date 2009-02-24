#include "global.h"
#include "input.h"

extern int flip;
int tabflag=0;
int plusflag=0;
int minusflag=0;
int playbyplay=0;

void ProcessInput()
{
	double stepsize=25;
	double anglesize=2;
	double newx;
	double newy;
	
	frametimer=GetTickCount()-frametimer;

	if(KEY_DOWN(VK_F1)) playbyplay=1;
	if(KEY_DOWN(VK_TAB))
	{
		if(tabflag==0)
		{
			if(mapinfo.show==0) mapinfo.show=1; else mapinfo.show=0;
			tabflag=1;
		}
	}

	if(KEY_DOWN(VK_ADD)) 
	{
		if(plusflag==0)
		{
			mapinfo.zoom*=1.5;
			plusflag=1;
		}
	}
	if(KEY_DOWN(VK_SUBTRACT)) 
	{
		if(minusflag==0)
		{
			mapinfo.zoom*=.66;
			minusflag=1;
		}
	}

/*	if(KEY_DOWN(VK_F11)) mapinfo.rotate=1;
	if(KEY_DOWN(VK_F12)) mapinfo.rotate=0;*/

	if(KEY_UP(VK_ADD)) plusflag=0;
	if(KEY_UP(VK_SUBTRACT)) minusflag=0;

	if(KEY_UP(VK_TAB)) tabflag=0;
	if(KEY_DOWN(VK_ESCAPE)) PostQuitMessage(0);
	if(KEY_DOWN(VK_PRIOR)) horizon+=20;
	if(KEY_DOWN(VK_NEXT)) horizon-=20;
	if(KEY_DOWN('A')) player.height+=.1;
	if(KEY_DOWN('Z')) player.height-=.1;
	if(KEY_DOWN(VK_RIGHT))
	{
		player.angle-=anglesize*frametimer/1000;
		if(player.angle<0) player.angle+=2*PI;
	}
	if(KEY_DOWN(VK_LEFT))
	{
		player.angle+=anglesize*frametimer/1000;
		if(player.angle>2*PI) player.angle-=2*PI;
	}
	if(KEY_DOWN(VK_UP))
	{
		newx=player.x+cos(player.angle)*stepsize*frametimer/1000;
		newy=player.y+sin(player.angle)*stepsize*frametimer/1000;
		DoWallCollisions(newx,newy);
	}
	if(KEY_DOWN(VK_DOWN))
	{
		newx=player.x-cos(player.angle)*stepsize*frametimer/1000;
		newy=player.y-sin(player.angle)*stepsize*frametimer/1000;
		DoWallCollisions(newx,newy);
	}
	
	frametimer=GetTickCount();	
}

