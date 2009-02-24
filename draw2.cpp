#include <math.h>
#include "global.h"
#include "macros.h"
#include "draw.h"

int surf=1;


int showmap=0;
int rotatemap=0;

double sinp, cosp;	
double ds0=1.5;
double w=1.25;
double w2=.625;
double w640=.001953295;
double hw=512;	
double px,py,p0x,p0y,p1x,p1y,ds;
int usepx;
int globalflag=1;

double g_u, g_v;

extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern int playbyplay;

LONG spacetimer;
int fy;

void pbpwait()
{
	int single;
	char buffer[100];

	if(playbyplay) 
	{
		while(GetTickCount()<spacetimer+1);

		single=0;
		while(KEY_DOWN(VK_CONTROL)) ;
		while(!KEY_DOWN(VK_SPACE) && !single)
		{
			if(KEY_DOWN(VK_CONTROL)) single=1;
			if(KEY_DOWN(VK_ESCAPE))
			{
				playbyplay=0;
				while(KEY_DOWN(VK_ESCAPE)) ;
				return;
			}
			if(KEY_DOWN(VK_RETURN))
			{ 
				wsprintf(buffer,"fy=%i\n",fy);
				DebugString(buffer);
				while(KEY_DOWN(VK_RETURN));
			}
		}

		spacetimer=GetTickCount();
	}
}

void LockBack()
{
	DDSURFACEDESC ddsd;
	DWORD ddrval;
	ddsd.dwSize=sizeof(DDSURFACEDESC);
	
	ddrval=lpDDSBack->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	//ddrval=lpDDSPrimary->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	//if(surf==2) ddrval=lpDDSBack2->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
	if(ddrval!=DD_OK) HandleBadResult(ddrval,0);
	vidmem=(UCHAR*)ddsd.lpSurface;
	pitch=ddsd.lPitch;
}

void UnlockBack()
{
	//lpDDSPrimary->Unlock(NULL);
	lpDDSBack->Unlock(NULL);
	//if(surf==2) lpDDSBack2->Unlock(NULL);
	vidmem=NULL;
	pitch=0;
}

void ClearBack()
{
	int x;
	for(x=0;x<130;x++) vline(x,0,VSIZE-1,0);

}

void vline(int x,int y0,int y1, DWORD color)
{
	int y;
	DWORD val;

	val=y0*pitch;
	for(y=y0;y<=y1;y++)
	{
		*(vidmem+val+4*x)=(UCHAR)color&0xFF;
		*(vidmem+val+4*x+1)=(UCHAR)(color>>8)&0xFF;
		*(vidmem+val+4*x+2)=(UCHAR)(color>>16)&0xFF;
		val+=pitch;
	}

  }

unsigned long dofade(unsigned long color,int fade)
{
	unsigned char r,g,b;

	r=color&0xFF;
			g=(color>>8)&0xFF;
			b=(color>>16)&0xFF;
			if(r>fade) r=r-fade;
			else r=0;
			
			if(g>fade) g=g-fade;
			else g=0;
			
			if(b>fade) b=b-fade;
			else b=0;

			color=b;
			color<<=8;
			color|=g;
			color<<=8;
			color|=r;

			return color;
	
}
void texwall(int x,int y0,int y1, int tex, int tx,int ty0,int ty1,int miny,int maxy, int fade=0)
{
	UCHAR *data;
	int width;
	int height;
	int i;
	int i2;
	int width3;
	int pitch4;
	int ib;
	int width3b;
	int ty;
	int error;
	int biginc;
	int max;

	unsigned char r,g,b;
	DWORD color;
#ifdef NOVIS
	return;
#endif

	pbpwait();
			
	width=(*(textures+tex))->width;
	height=(*(textures+tex))->height;
	data=(*(textures+tex))->data;
	
	__asm {
			mov eax,pitch;	//pitch4=pitch-4;
			sub eax,4;
			mov pitch4,eax;
			
			mov error,0;	//error=0;
			
			mov eax,ty0;	//ty=ty0;
			mov ty,eax;
			mov eax,y1;		//i=y1-y0;
			sub eax,y0;
			mov i,eax;
		
			mov eax,ty1;	//i2=ty1-ty0;
			sub eax,ty0;
			mov i2,eax;

			mov eax,y0;		//if(y0<miny) {
			cmp eax,miny;
			jge noadj;
			
			mov eax,miny;   //error=i2*(miny-y0)%i;
			sub eax,y0;
			mov edx,i2;
			mul edx;
			xor edx,edx;
			mov ebx,i;
			div ebx;
			mov error,edx;

			xchg eax,edx;	//ty=ty+i2*(miny-y0)/i;
			mov eax,ty;
			add eax,edx;
			mov ty,eax;
			mov eax,miny;	//y0=miny;
			mov y0,eax;
noadj:						//}
	
			mov eax,width;	//width3=width*3;
			shl eax,1;
			add eax,width;
			mov width3,eax;
			
			mov eax,width3; //biginc=width3*height;
			mov edx,height;
			mul edx;
			mov biginc,eax;

			mov eax,i;		//ib=i;
			mov ib,eax;
			
			mov eax,width3;	//width3b=width3;
			mov width3b,eax;
			
			cmp i2,0;		//if(i2!=0) {
			je noopt;
			
			xor edx,edx;	//width3b=width3*i/i2;
			mov eax,i;
			mov ebx,i2;
			div ebx;
			mov ecx,eax;
			mov ebx,width3;
			mul ebx;
			mov width3b,eax;

			mov eax,ecx;	//ib=i*i/i2;
			mov ebx,i;
			mul ebx;

			cmp eax,0;		//if(ib==0) ib=1;
			jne noadd;
			mov eax,1;
noadd:
			mov ib,eax;
noopt:						//}
			mov esi,data;	//esi=data+3*width*ty+tx*3;
			mov eax,tx;
			mov edx,3;
			mul edx;
			add esi,eax;
			mov eax,width3;
			mov edx,ty;
			mul edx;
			add esi,eax;
			
			mov edi,vidmem; //edi=vidmem+y0*pitch+x*4;
			mov eax,x;
			shl eax,2;
			add edi,eax;
			mov eax,y0;
			mov edx,pitch;
			mul edx;
			add edi,eax;

			xor edx,edx;	//max=data+3*width*height;
			mov eax,width3; 
			mov ecx,height;
			mul ecx;
			add eax,data
			mov max,eax;
			
			mov eax,y1;
			cmp eax,maxy;	//if(y1>maxy) y1=maxy;
			jle noadj2;
			mov eax,maxy;
			mov y1,eax;
noadj2:
			mov ecx,y1;		//ecx=y1-y0+1;
			sub ecx,y0;
			inc ecx;

			mov eax,error;	//eax=error;
			mov ebx,i;		//ebx=i;
			
							//inner loop
label:		cmp eax,ib;		//if(eax>ib) 
			jg changeline;
back:		cmp eax,ebx;	//if(eax>ebx)
			jg changeline2;
change:
			cmp esi,max;	//if(esi>max)
			jge fixbig;
			cmp esi,data;	//if(esi<data)
			jl fixlittle;
nofix:		movsd;			//pixel write
			add edi,pitch4; //edi+=pitch4;
			sub esi,4;		//esi-=4;
			add eax,i2;		//eax+=i2;
			loop label;
	
			jmp done;
changeline:	sub eax,ib;		//eax-=ib;
			add esi,width3b;//esi+=width3b;
			jmp back;
changeline2:sub eax,ebx;	//eax-=ebx;
			add esi,width3; //esi+=width3;
			jmp back;
fixbig:		sub esi,biginc;	//esi-=biginc;
			jmp change;
fixlittle:	add esi,biginc; //esi+=biginc;
			jmp change;
done:
	}
		
  }
void hline(int x0,int x1,int y, int r, int g, int b)
{
	int x;
	DWORD val;
	
	
#ifdef NOVIS
	return;
#endif

	val=y*pitch;
	for(x=x0;x<=x1;x++)
	{
		*(vidmem+val+x*4)=b;
		*(vidmem+val+x*4+1)=g;
		*(vidmem+val+x*4+2)=r;
	}

  }

void getuvfloor(int y,struct Sector *sector,double data1,double data2,int datatype)
{
	double mpy,yp,s;
	double lpx,lpy,ix,iy,ipx,ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	if(datatype==0)
	{
		ix=data1;
		iy=data2;
	}
	else
	{
	mpy=player.height-sector->floorheight;
	yp=(y-horizon)/hw;
	s=mpy/yp;
		
	ix=data1*s+p0x;
	iy=data2*s+p0y;
	}

	ipx=ix-sector->walls->start.x;
	ipy=sector->walls->start.y-iy;
	
	g_u=ipx*worldtotex;
	g_v=ipy*worldtotex;
}

void getuvceil(int y,struct Sector *sector,double data1,double data2,int datatype)
{
	double mpy,yp,s;
	double lpx,lpy,ix,iy,ipx,ipy;

	//lpx=ds0*cosp-sinp*(w2+x*w640);
	//lpy=ds0*sinp+cosp*(w2-x*w640);
	if(datatype==0)
	{
		ix=data1;
		iy=data2;
	}
	else
	{
	mpy=sector->ceilingheight-player.height;
	yp=(horizon-y)/hw;
	s=mpy/yp;
		
	ix=data1*s+p0x;
	iy=data2*s+p0y;
	}

	ipx=ix-sector->walls->start.x;
	ipy=sector->walls->start.y-iy;
	
	g_u=ipx*worldtotex;
	g_v=ipy*worldtotex;
}

void texfloorceil(int x0,int x1,int y,struct Sector *sector,int tex,double u0,double v0,double u1,double v1)
{
	double mpy,yp,s;
	double ix,iy;
	double ipx,ipy;
	double lpx,lpy;
	//double u0,v0,u1,v1;
	double ua,va;
	int u,v,ud,vd;
	int x;
	int width,height;
	int uerror,verror;
	int ui,vi,ui2,vi2;
	int um,vm,uxm,vxm;
	char buffer[100];
	UCHAR *data;
	UCHAR *val, *val2;
	int width3;
	int biginc,littleinc;
	int screenwidth;

	//wsprintf(buffer,"texfloorceil %i-%ix%i, %ix%i-%ix%i\n",x0,x1,y,(int)u0,(int)v0,(int)u1,(int)v1);
	//DebugString(buffer);
	
	pbpwait();
			
	data=(*(textures+tex))->data;
	width=(*(textures+tex))->width;
	height=(*(textures+tex))->height;
	
	width3=width*3;
	biginc=width3*height;
	littleinc=width3;

	if(u0>0) u=u0;
	else	 u=u0-1;
	if(v0>0) v=v0;
	else     v=v0-1;
	
	ud=u1-u0;
	vd=v1-v0;
	if(ud==0) 
	{
		if(u1>u0) ud=1; 
		else ud=-1;
	}
	if(vd==0)
	{
		if(v1>v0) vd=1; 
		else vd=-1;
	}

	um=abs(ud);
	vm=abs(vd);

	uxm=abs((double)ud*(x1-x0)/(u1-u0));
	if(uxm==0) uxm=(x1-x0);
	
	vxm=abs((double)vd*(x1-x0)/(v1-v0));
	if(vxm==0) vxm=(x1-x0);
	
	if(ud>0) uerror=abs((u-u0)*uxm);
	else	 uerror=abs((u-u0+1)*uxm);

	if(vd>0) verror=abs((v-v0)*vxm);
	else	 verror=abs((v-v0+1)*vxm);
	
	
	if(u1>u0) ui=1; 
	else	  ui=-1; 
		
	if(v1>v0) vi=1; 
	else      vi=-1; 
	
	ui2=ui*3;
	vi2=vi*width3;

	/*
	val=vidmem+y*pitch+x0*4;
	val2=data+v*width3+u*3;
	for(x=0;x<=x1-x0;x++)
	{
	while(uerror>uxm) 
		{ 
			u+=ui;
			val2+=ui2;
			uerror-=uxm; 
		}
		while(verror>vxm) 
		{ 
			v+=vi; 
			val2+=vi2;
			verror-=vxm; 
		}
			
		while(u<0) { u+=width; val2+=littleinc; }
		while(u>=width) { u-=width; val2-=littleinc; }
		while(v<0) { v+=height; val2+=biginc; }
		while(v>=height) { v-=height; val2-=biginc; }
	
		*(val)=*(val2);
		*(val+1)=*(val2+1);
		*(val+2)=*(val2+2);
		
		val+=4;

		uerror+=um;
		verror+=vm;

		screenwidth--;
		if(screenwidth>0) goto biglooptop;
		}*/
		__asm {
	
		mov edi,vidmem;
		mov eax,y;
		mov ebx,pitch;
		mul ebx;
		add edi,eax;
		mov eax,x0;
		mov ebx,4;
		mul ebx;
		add edi,eax;

		mov esi,data;
		mov eax,v;
		mov ebx,width3;
		mul ebx;
		add esi,eax;
		mov eax,u;
		mov ebx,3;
		mul ebx;
		add esi,eax;

		mov ecx,x1;
		sub ecx,x0;
		inc ecx;
biglooptop:
		
ulooptop:
		mov eax,uerror;
		cmp eax,uxm;
		jg ufix;
		jmp vlooptop;
ufix:
		mov eax,uerror;
		sub eax,uxm;
		mov uerror,eax;
		mov eax,u;
		add eax,ui;
		mov u,eax;
		add esi,ui2;
		jmp ulooptop;
vlooptop:
		mov eax,verror;
		cmp eax,vxm;
		jg vfix;
		jmp umintop;
vfix:
		mov eax,verror;
		sub eax,vxm;
		mov verror,eax;
		mov eax,v;
		add eax,vi;
		mov v,eax;
		add esi,vi2;
		jmp vlooptop;
umintop:
		mov eax,u;
		cmp eax,0;
		jl uminfix;
umaxtop:
		mov eax,u;
		cmp eax,width;
		jge umaxfix;
vmintop:
		mov eax,v;
		cmp eax,0;
		jl vminfix;
vmaxtop:
		mov eax,v;
		cmp eax,height;
		jge vmaxfix;
		jmp nomodfix;
uminfix:
		mov eax,width;
		add u,eax;
		add esi,littleinc;
		jmp umintop;
umaxfix:
		mov eax,width;
		sub u,eax;
		sub esi,littleinc;
		jmp umaxtop;
vminfix:
		mov eax,height;
		add v,eax;
		add esi,biginc;
		jmp vmintop;
vmaxfix:
		mov eax,height;
		sub v,eax;
		sub esi,biginc;
		jmp vmaxtop;
nomodfix:
		
		movsd;
		sub esi,4;

		mov eax,um;
		add uerror,eax;
		mov eax,vm;
		add verror,eax;
		dec ecx;
		cmp ecx,0;
		jg biglooptop;
	}
		
}
void DrawScreen()
{
#ifndef NOVIS
	ClearSurface(lpDDSBack);
#endif

	sinp=sin(player.angle);
	cosp=cos(player.angle);
	DrawWalls();
	if(mapinfo.show) DrawOverhead();
	
}

void DrawOverhead()
{
	int i,j;
	struct Sector *sector;
	struct Wall *wall;
	int x0,y0,x1,y1;
	int x0b,y0b,x1b,y1b;
	int x0c,y0c,x1c,y1c;
	
	if(mapinfo.rotate==0) line(320,240,320+cosp*20,240-sinp*20,0xFF0000);
	else				  line(320,240,320,220,0xFF0000);
	circle(320,240,7,0xFF0000);

	for(i=0;i<numsectors;i++)
	{
		sector=sectors+i;
		for(j=0;j<sector->numwalls;j++)
		{
			wall=sector->walls+j;
			x0b=(wall->start.x-player.x)*mapinfo.zoom+320;
			y0b=-(wall->start.y-player.y)*mapinfo.zoom+240;
			x1b=(wall->end.x-player.x)*mapinfo.zoom+320;
			y1b=-(wall->end.y-player.y)*mapinfo.zoom+240;

			if(mapinfo.rotate)
			{
				x0c=x0b-320;
				y0c=y0b-240;
				x1c=x1b-320;
				y1c=y1b-240;
	
				x0=320+x0c*sinp+y0c*cosp;
				y0=240+y0c*sinp-x0c*cosp;
				x1=320+x1c*sinp+y1c*cosp;
				y1=240+y1c*sinp-x1c*cosp;
			}
			else
			{
				x0=x0b;
				y0=y0b;
				x1=x1b;
				y1=y1b;
			}
	
			if(wall->flags==WALL_NORMAL) line(x0,y0,x1,y1,0x00FF00);
			else						 line(x0,y0,x1,y1,0x008000);

			vertex(x0,y0);
		}
	}
	
}

void vertex(int x,int y)
{
	int i,j;
	DWORD color;

	if(pointselected && screenpoint.x==x && screenpoint.y==y) color=0xFFFFFF;
	else									 color=0xFFFF00;
	for(i=x-2;i<x+2;i++)
		for(j=y-2;j<y+2;j++)
			PutPixel(i,j,color);
}
void line(int x0,int y0,int x1,int y1,DWORD color)
{
	int xd,yd;
	int error;
	int x,y;
	int xi1,xi2,yi1,yi2;
	int x0c,y0c,x1c,y1c;
	int e1,e2;
	int i,i2;
	
	xd=abs(x1-x0);
	yd=abs(y1-y0);
	x=x0;
	y=y0;
	
	if(x1>x0) xi1=1; else xi1=-1;
	if(y1>y0) yi1=1; else yi1=-1;
	
	i=0;
	if(xd>yd) 
	{ 
		i2=xd;
		error=xd/2;
		e1=yd;
		e2=xd;
		xi2=xi1; 
		yi2=0;
	} 
	else
	{
		i2=yd;
		error=yd/2;
		e1=xd;
		e2=yd;
		xi2=0;
		yi2=yi1;
	}

	while(i!=i2)
	{
		PutPixel(x,y,color);
		error+=e1;
		if(error>e2)
		{
			error-=e2;
			x+=xi1;
			y+=yi1;
		}
		else 
		{
			x+=xi2;
			y+=yi2;
		}
		i++;
	}
}

void circle(int cx, int cy, int rad, DWORD color)
{
	int d;
	int x,y;

	d=3-(2*rad);
	x=0;
	y=rad;

	while(1)
	{
		PutPixel(cx+x,cy+y,color);
		PutPixel(cx-x,cy+y,color);
		PutPixel(cx+x,cy-y,color);
		PutPixel(cx-x,cy-y,color);
		PutPixel(cx+y,cy+x,color);
		PutPixel(cx+y,cy-x,color);
		PutPixel(cx-y,cy+x,color);
		PutPixel(cx-y,cy-x,color);
		if(x==y || x==rad) break;
		if(d<0) d+=4*x+6;
		else
		{
			d+=4*(x-y)+10;
			y--;
		}
		x++;
		
	}
}
void PutPixel(int x, int y, DWORD color)
{
	if(x<0 || x>HSIZE-1 || y<0 || y>VSIZE-1) return;
	*(vidmem+y*pitch+x*4)=color&0xFF;
	*(vidmem+y*pitch+x*4+1)=(color>>8)&0xFF;
	*(vidmem+y*pitch+x*4+2)=(color>>16)&0xFF;

}
FloorCeilingInfo floors[VSIZE];

void InitWalls()
{
	int i;

	for(i=0;i<VSIZE;i++) floors[i].x=-1;
}

void DrawWalls()
{
	int x,y,i;
	double xinc,yinc;
	double u0,v0,u1,v1;
	struct Sector* sector;
	char buffer[100];

	spacetimer=GetTickCount();
	fy=VSIZE-1;
	
	//for(i=0;i<HSIZE;i++) vline(i,0,VSIZE-1,0xFF00FF);

	p0x=player.x;
	p0y=player.y;
	p1x=ds0*cosp-sinp*w2+p0x;
	p1y=ds0*sinp+cosp*w2+p0y;
	xinc=w640*sinp;
	yinc=-w640*cosp;
	numactivesectors=0;

	for(x=0;x<HSIZE;x++) 
	{
		px=p1x-p0x;
		py=p1y-p0y;
		
		if(abs(px)>abs(py))
		{ 
			usepx=1; 
			ds=hw*px; 
		}
		else
		{ 
			usepx=0; 
			ds=hw*py; 
		}

		if(ubermode==0) DrawWallSlice(x,sectors+player.sector,-1,0,VSIZE-1);
		else			DrawWallSlice(x,sectors+player.sector,-1,0,VSIZE-1);
		p1x+=xinc;
		p1y+=yinc;		
	}
	int k;

	for(k=fy+1;k<VSIZE;k++)
	{
		if(k<0) continue;
		if(floors[k].x==-1) continue;

		getuvfloor(k,floors[k].sector,px,py,1);
		pbpwait();
		texfloorceil(floors[k].x,x,k,floors[k].sector,floors[k].sector->floortex,floors[k].u,floors[k].v,g_u,g_v);
		floors[k].x=-1;
	}
/*

	for(i=0;i<numactivesectors;i++)
	{
		sector=sectors+*(activesectors+i);
		for(y=sector->minfloory;y<=sector->maxfloory;y++)
		if(sector->floor[y]!=-1) 
		{
			getuvfloor(y,sector,sector->px,sector->py,1);
			u1=g_u;
			v1=g_v;
			texfloorceil(sector->floor[y],sector->maxx,y,sector,sector->floortex,us[sector->floor[y]][y],vs[sector->floor[y]][y],u1,v1);
			sector->floor[y]=-1;	
		}
		if(sector->maxceily>=sector->minceily)
		{
		for(y=sector->minceily;y<=sector->maxceily;y++)
		if(sector->ceil[y]!=-1) 
		{
			getuvceil(y,sector,sector->px,sector->py,1);
			u1=g_u;
			v1=g_v;
			texfloorceil(sector->ceil[y],sector->maxx,y,sector,sector->ceiltex,us[sector->ceil[y]][y],vs[sector->ceil[y]][y],u1,v1);
			sector->ceil[y]=-1;	
		}
		}	
	}
	*/
	if(globalflag<65536) globalflag++;
	else globalflag=0;
	playbyplay=0;
}	

void DrawFloors()
{
	int y;
	for(y=VSIZE-1;y>=horizon;y--) DrawFloorSlice(y,sectors+player.sector,0,HSIZE-1);
}

void DrawCeilings()
{
	int y;
	for(y=0;y<horizon;y++) DrawCeilingSlice(y,sectors+player.sector,0,HSIZE-1);
}
//#define DebugFloors

void DrawFloorSlice(int y,struct Sector *sector,int minx,int maxx)
{
	return;
	/*
	int startx,endx;
	int breakall=0;
	int wall;

	if(sector==0) DebugString("Attempt to access null sector\n");
	startx=endx=-1;
	startx=minx;
	#ifdef DebugFloors
	wsprintf(buffer,"Drawing floor slice %i in sector %i, bounds %i-%i\n",y,sector,minx,maxx);
    DebugString(buffer); 
	#endif
	while(1)
	{
	if(sector->floor[startx]>=y)
	{

		while(1)
		{
		#ifdef DebugFloors
		DebugString("Searching for valid adjoins\n"); 
		#endif
		breakall=1;
		for(;startx<=maxx;startx++)
		{
			if(sector->floor[startx]<y)
			{
				breakall=1;
				#ifdef DebugFloors
				wsprintf(buffer,"Aborting adjoin search, floor found at x=%i\n",startx); 
				DebugString(buffer); 
				#endif
				break;
			}
			if(sector->adjbott[startx]==-1) continue;
			if(sector->adjbott[startx]>=y) 
			{
				breakall=0;
				wall=sector->wall[startx];
				#ifdef DebugFloors	
				wsprintf(buffer,"Adjoin of %i found at x=%i, wall %i\n",sector->adjbott[startx],startx,wall);
				DebugString(buffer); 
				#endif
				break;
			}
		}
		if(breakall) break;
		#ifdef DebugFloors
		DebugString("Searching for end of adjoin\n"); 
		#endif
		for(endx=startx;endx<=maxx;endx++)
		{
			if(sector->adjbott[endx]<y)
			{
				#ifdef DebugFloors
				wsprintf(buffer,"Adjoin end found at x=%i\n",endx);
				DebugString(buffer); 
				#endif
				endx--;
				break;
			}
			if(sector->wall[endx]!=wall)
			{
				#ifdef DebugFloors	
				wsprintf(buffer,"Wall change found at x=%i\n",endx);
				DebugString(buffer); 
				#endif
				endx--;
				break;
			}
		}
		if(endx==maxx+1) endx=maxx;
		#ifdef DebugFloors
		wsprintf(buffer,"Recursing DrawFloorSlice() for sector %i, %i-%i\n",(sector->walls+wall)->adjoin,startx,endx);
		DebugString(buffer); 
		#endif
		DrawFloorSlice(y,sectors+(sector->walls+wall)->adjoin,startx,endx);
		startx=endx+1;
		endx=-1;
		}
	}
	if(startx==maxx+1) return;
	#ifdef DebugFloors	
	DebugString("Searching for floor\n"); 
	#endif
	for(;startx<=maxx;startx++)
	{
		if(sector->floor[startx]<y)
		{
			break;
		}
	}
	if(startx==maxx+1) return;
	for(endx=startx;endx<=maxx;endx++)
	{
		if(sector->floor[endx]>=y)
		{
			endx--;
			break;
		}
		if(sector->adjbott[endx]==-1) continue;
		if(sector->adjbott[endx]>y)
		{
			endx--;
			break;
		}
	}
	if(endx==maxx+1) endx=maxx;
//	hline(startx,endx,y,0,255,0);
//	texfloorceil(startx,endx,y,sector);
	startx=endx+1;
	if(startx==maxx+1) return;
	}
	*/
}

//#define DebugCeilings

void DrawCeilingSlice(int y,struct Sector *sector,int minx,int maxx)
{

	return;
	/*
	int startx,endx;
	int breakall=0;
	int wall;

	if(sector==0) DebugString("Attempt to access null sector\n");
	startx=endx=-1;
	startx=minx;
	#ifdef DebugCeilings
	wsprintf(buffer,"Drawing floor slice %i in sector %i, bounds %i-%i\n",y,sector,minx,maxx);
    DebugString(buffer); 
	#endif
	while(1)
	{

	if(sector->ceiling[startx]<=y)
	{

		while(1)
		{
		#ifdef DebugCeilings
		DebugString("Searching for valid adjoins\n"); 
		#endif
		breakall=1;
		for(;startx<=maxx;startx++)
		{
			if(sector->ceiling[startx]>y)
			{
				breakall=1;
				#ifdef DebugCeilings
				wsprintf(buffer,"Aborting adjoin search, floor found at x=%i\n",startx); 
				DebugString(buffer); 
				#endif
				break;
			}
			if(sector->adjtop[startx]==-1) continue;
			if(sector->adjtop[startx]<=y) 
			{
				breakall=0;
				wall=sector->wall[startx];
				#ifdef DebugCeilings	
				wsprintf(buffer,"Adjoin of %i found at x=%i, wall %i\n",sector->adjtop[startx],startx,wall);
				DebugString(buffer); 
				#endif
				break;
			}
		}
		if(breakall) break;
		#ifdef DebugCeilings
		DebugString("Searching for end of adjoin\n"); 
		#endif
		for(endx=startx;endx<=maxx;endx++)
		{
			if(sector->adjtop[endx]>y)
			{
				#ifdef DebugCeilings
				wsprintf(buffer,"Adjoin end found at x=%i\n",endx);
				DebugString(buffer); 
				#endif
				endx--;
				break;
			}
			if(sector->wall[endx]!=wall)
			{
				#ifdef DebugCeilings	
				wsprintf(buffer,"Wall change found at x=%i\n",endx);
				DebugString(buffer); 
				#endif
				endx--;
				break;
			}
		}
		if(endx==maxx+1) endx=maxx;
		#ifdef DebugCeilings
		wsprintf(buffer,"Recursing DrawCeilingSlice() for sector %i, %i-%i\n",(sector->walls+wall)->adjoin,startx,endx);
		DebugString(buffer); 
		#endif
		DrawCeilingSlice(y,sectors+(sector->walls+wall)->adjoin,startx,endx);
		startx=endx+1;
		endx=-1;
		}
	}
	if(startx==maxx+1) return;
	#ifdef DebugCeilings	
	DebugString("Searching for ceiling\n"); 
	#endif
	for(;startx<=maxx;startx++)
	{
		if(sector->ceiling[startx]>y)
		{
			break;
		}
	}
	if(startx==maxx+1) return;
	for(endx=startx;endx<=maxx;endx++)
	{
		if(sector->ceiling[endx]<=y)
		{
			endx--;
			break;
		}
		if(sector->adjtop[endx]==-1) continue;
		if(sector->adjtop[endx]<y)
		{
			endx--;
			break;
		}
	}
	if(endx==maxx+1) endx=maxx;
	
	hline(startx,endx,y,0,255,0);

	startx=endx+1;
	if(startx==maxx+1) return;
	}
	*/
}



void DrawWallSlice(int x,struct Sector *sector,int ignore,int miny,int maxy)
{
	struct Wall tempwall;
	double q0x,q0y,q1x,q1y;
	double qx,qy,pqx,pqy;
	double s,d,bottom,dsd;
	double u0,v0,u1,v1;
	double ix,iy;
	int h1t,h2t,h1bt,h2bt;
	int h1i,h2i,h1bi,h2bi;
	int i,wall,j;
	int toptx,midtx,botttx;
	int toptex,midtex,botttex;
	char buffer[100];
	int y0,y1;
	int keepgoing=1;
	int k;

	while(keepgoing)
	{
	if(sector->lastwall!=-1) wall=sector->lastwall-1;
	else					 wall=-1;
	
	for(i=0;i<sector->numwalls;i++)
	{
		wall++;		
		if(wall>=sector->numwalls) wall%=sector->numwalls;
		if(wall==ignore) continue;
		tempwall=*(sector->walls+wall);
		
		q0x=tempwall.start.x;
		q0y=tempwall.start.y;
		q1x=tempwall.end.x;
		q1y=tempwall.end.y;
		
		qx=q1x-q0x;
		qy=q1y-q0y;
		pqx=p0x-q0x;
		pqy=p0y-q0y;
		
		bottom=px*qy-py*qx;
		s=(px*pqy-py*pqx)/bottom;
		if(s<0.0 || s>1.0) continue;
	
		ix=q0x+qx*s;
		iy=q0y+qy*s;
		if(usepx) d=ix-p0x;
		else   	  d=iy-p0y;
		
		dsd=ds/d;
		if(dsd>0.0) break;
	}
		h1i=horizon+(int)(dsd*(player.height-sector->floorheight));
		h2i=horizon-(int)(dsd*(sector->ceilingheight-player.height));
		
		if(sector->beenin!=globalflag)
		{
			sector->fy=maxy;
			sector->fy2=maxy;
			sector->lastwall=-1;

			sector->maxfloory=maxy;
			sector->minfloory=h1i+1;
			sector->minceily=miny;
			sector->maxceily=h2i-1;
		
			if(sector->minfloory<0) sector->minfloory=0;
			if(sector->maxfloory>VSIZE-1) sector->maxfloory=VSIZE-1;
			if(sector->minceily<0) sector->minceily=0;
			if(sector->maxceily>VSIZE-1) sector->maxceily=VSIZE-1;
			
			if(sector->minfloory<miny) sector->minfloory=miny;
			if(sector->maxceily>maxy) sector->maxceily=maxy;

			if(maxy>=horizon)
			{
				y0=sector->minfloory;
				if(y0<VSIZE)
				{
					y1=sector->maxfloory;
					if(y1<0) y1=0;
					if(y1>VSIZE-1) y1=VSIZE-1;
					if(y0<0) y0=0;
					for(j=y0;j<=y1;j++) 
					{
						getuvfloor(j,sector,px,py,1);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->floor[j]=x;
					}
				}
			}
			if(miny<=horizon)
			{
				y0=sector->minceily;
				if(y0<VSIZE)
				{
					y1=sector->maxceily;
					if(y1<0) y1=0;
					if(y1>VSIZE-1) y1=VSIZE-1;
					if(y0<0) y0=0;
					for(j=y0;j<=y1;j++) 
					{
						getuvceil(j,sector,px,py,1);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->ceil[j]=x;
					}
				}
			}
			
			sector->beenin=globalflag;
			*(activesectors+numactivesectors++)=sector-sectors;
		}
		
		sector->maxx=x;	
		sector->px=px;
		sector->py=py;

		
		
		if(tempwall.flags==WALL_NORMAL)
		{
			for(k=fy+1;k<=sector->fy;k++)
			{
				if(k<miny) continue;
				if(k>maxy) break;
				if(floors[k].x==-1) continue;

				getuvfloor(k,floors[k].sector,px,py,1);
				texfloorceil(floors[k].x,x-1,k,floors[k].sector,floors[k].sector->floortex,floors[k].u,floors[k].v,g_u,g_v);
				floors[k].x=-1;
			}
			fy=sector->fy;
		}

		if(h1i>sector->fy)
		{
			for(k=sector->fy+1;k<=h1i;k++)
			{
				if(k<miny) continue;
				if(k>maxy) break;
				if(floors[k].x==-1) continue;

				getuvfloor(k,sector,px,py,1);
				texfloorceil(floors[k].x,x-1,k,floors[k].sector,floors[k].sector->floortex,floors[k].u,floors[k].v,g_u,g_v);
				floors[k].x=-1;
			}
			sector->fy=h1i;
			if(tempwall.flags==WALL_NORMAL) fy=h1i;
		}

		if(maxy<sector->fy2)
		{
			for(k=maxy+1;k<=sector->fy2;k++)
			{
				if(floors[k].x==-1) continue;
				if(floors[k].sector!=sector) continue;
				getuvfloor(k,sector,px,py,1);
				texfloorceil(floors[k].x,x-1,k,floors[k].sector,floors[k].sector->floortex,floors[k].u,floors[k].v,g_u,g_v);
				floors[k].x=-1;
			}
			sector->fy2=maxy;
		}

		if(h1i<sector->fy)
		{
			for(k=h1i+1;k<=sector->fy;k++)
			{
				if(k<miny) continue;
				if(k>maxy) break;

				if(floors[k].x!=-1)
				{
					getuvfloor(k,floors[k].sector,px,py,1);
					texfloorceil(floors[k].x,x-1,k,floors[k].sector,floors[k].sector->floortex,floors[k].u,floors[k].v,g_u,g_v);
				}

				floors[k].x=x;
				floors[k].sector=sector;
				getuvfloor(k,sector,px,py,1);
				floors[k].u=g_u;
				floors[k].v=g_v;
			}
			sector->fy=h1i;
			fy=h1i;
		}

		if(maxy>sector->fy2)
		{
			for(k=sector->fy2+1;k<=maxy;k++)
			{
				if(k<miny) continue;

				floors[k].x=x;
				floors[k].sector=sector;
				getuvfloor(k,sector,px,py,1);
				floors[k].u=g_u;
				floors[k].v=g_v;
			}
			sector->fy2=maxy;
		}
		
		

		sector->lastwall=wall;
		
		
	/*
		// begin floor stuff
		if(maxy>=horizon)
		{
			y0=sector->minfloory;
			y1=h1i+1;
			if(y0>=0 && y1<VSIZE)
			{
				if(y0<horizon) y0=horizon;
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y1<0) y1=0;
				if(y1<horizon) y1=horizon;
				if(y1>VSIZE-1) y1=VSIZE-1;
				if(y1<miny) y1=miny;

				for(j=y0;j>=y1;j--) 
					if(sector->floor[j]==-1) 
					{
						if(j!=h1i) getuvfloor(j,sector,px,py,1);
						else getuvfloor(j,sector,ix,iy,0);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->floor[j]=x;
						if(j<sector->minfloory) sector->minfloory=j;
					}
			}
			
			y0=sector->maxfloory+1;
			y1=maxy;
			if(y0<VSIZE && y1>=0)
			{
				if(y0<0) y0=0;
				if(y0<horizon) y0=horizon;
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y1<horizon) y1=horizon;
				if(y1>VSIZE-1) y1=VSIZE-1;

				for(j=y0;j<=y1;j++) 
					if(sector->floor[j]==-1) 
					{
						if(j!=h1i) getuvfloor(j,sector,px,py,1);
						else getuvfloor(j,sector,ix,iy,0);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->floor[j]=x;
					
						if(j>sector->maxfloory) sector->maxfloory=j;
					}
			}
		
			y0=h1i;
			if(y0<miny) y0=miny;
			y1=sector->minfloory;
			if(y1<VSIZE && y0>=0)
			{
				if(y0<horizon) y0=horizon;
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y1<miny) y1=miny;
				if(y1<0) y1=0;
				if(y1<horizon) y1=horizon;
				if(y1>VSIZE-1) y1=VSIZE-1;
	
				for(j=y0;j>=y1;j--)
				{	
					if(sector->floor[j]!=-1)
					{
						if(j!=h1i) getuvfloor(j,sector,px,py,1);
						else getuvfloor(j,sector,ix,iy,0);
						u1=g_u;
						v1=g_v;
						texfloorceil(sector->floor[j],x,j,sector,sector->floortex,us[sector->floor[j]][j],vs[sector->floor[j]][j],u1,v1);
						sector->floor[j]=-1;
					}
					else break;
				}
			}
		
			y0=maxy+1;
			y1=sector->maxfloory;
			if(y0<VSIZE && y1>=0)
			{
				if(y0<0) y0=0;
				if(y1>VSIZE-1) y1=VSIZE-1;

				for(j=y0;j<=y1;j++)
				{	
					if(sector->floor[j]!=-1)
					{
						if(j!=h1i) getuvfloor(j,sector,px,py,1);
						else getuvfloor(j,sector,ix,iy,0);
						u1=g_u;
						v1=g_v;
						texfloorceil(sector->floor[j],x,j,sector,sector->floortex,us[sector->floor[j]][j],vs[sector->floor[j]][j],u1,v1);
						sector->floor[j]=-1;
					}
					else break;
				}
			}
		}
		// end floor stuff

		//begin ceiling stuff
		if(miny<=horizon)
		{
			y0=sector->maxceily;
			y1=h2i-1;
			if(y0<VSIZE && y1>=0)
			{
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y0>horizon) y0=horizon;
				if(y1>horizon) y1=horizon;
				if(y0<0) y0=0;
				if(y1<0) y1=0;
				if(y1>maxy) y1=maxy;
				if(y1>VSIZE-1) y1=VSIZE-1;
			
				for(j=y0;j<=y1;j++) 
					if(sector->ceil[j]==-1) 
					{
						if(j!=h2i) getuvceil(j,sector,px,py,1);
						else getuvceil(j,sector,ix,iy,0);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->ceil[j]=x;
						if(j>sector->maxceily) sector->maxceily=j;
					}
			}
		
			y0=sector->minceily-1;
			y1=miny;
			if(y0>=0 && y1<VSIZE)
			{
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y0>horizon) y0=horizon;
				if(y0<0) y0=0;
				if(y1>horizon) y1=horizon;
				if(y1<0) y1=0;
				if(y1>VSIZE-1) y1=VSIZE-1;

				for(j=y0;j>=y1;j--) 
					if(sector->ceil[j]==-1) 
					{
						if(j!=h2i) getuvceil(j,sector,px,py,1);
						else getuvceil(j,sector,ix,iy,0);
						us[x][j]=g_u;
						vs[x][j]=g_v;
						sector->ceil[j]=x;
						if(j<sector->minceily) sector->minceily=j;
					}
				}
	
	
			y0=h2i;
			if(y0>maxy) y0=maxy;
			y1=sector->maxceily;
		
			if(y0<VSIZE && y1>=0)
			{
				if(y1>VSIZE-1) y1=VSIZE-1;
				if(y0>horizon) y0=horizon;
				if(y0<0) y0=0;
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y1>horizon) y1=horizon;
				if(y1>maxy) y1=maxy;
				if(y1<0) y1=0;
					
				for(j=y0;j<=y1;j++)
				{
					if(sector->ceil[j]!=-1)
					{
						if(j!=h2i) getuvceil(j,sector,px,py,1);
						else getuvceil(j,sector,ix,iy,0);
						u1=g_u;
						v1=g_v;
						texfloorceil(sector->ceil[j],x,j,sector,sector->ceiltex,us[sector->ceil[j]][j],vs[sector->ceil[j]][j],u1,v1);
						sector->ceil[j]=-1;
					}
					else break;
				
				}
			}

			y0=miny-1;
			y1=sector->minceily;
			if(y0>=0 && y1<VSIZE) 
			{
				if(y0>VSIZE-1) y0=VSIZE-1;
				if(y1<0) y1=0;

				for(j=y0;j>=y1;j--)
				{
					if(sector->ceil[j]!=-1)
					{
						if(j!=h2i) getuvceil(j,sector,px,py,1);
						else getuvceil(j,sector,ix,iy,0);
						u1=g_u;
						v1=g_v;
						texfloorceil(sector->ceil[j],x,j,sector,sector->ceiltex,us[sector->ceil[j]][j],vs[sector->ceil[j]][j],u1,v1);
						sector->ceil[j]=-1;
					}
					else break;
				}
			}
		}
		//wsprintf(buffer,"minfloor=%i, maxfloor=%i, minceil=%i, maxceil=%i\n",sector->minfloory,sector->maxfloory,sector->minceily,sector->maxceily);
		//DebugString(buffer);
		//end ceiling stuff
*/
		if(h2i>maxy)
		{
			//sector->ceiling[x]=maxy;
			//sector->adjtop[x]=-1;
			return;
		}
		if(h1i<miny) 
		{
			//sector->ceiling[x]=miny;
			//sector->adjtop[x]=-1;
			return;
		}
		if(tempwall.flags==WALL_NORMAL)
		{
			h2t=tempwall.midoffset;
			h1t=tempwall.midbott;
			midtex=tempwall.midtex;
			midtx=tempwall.midhoriz+(int)(s*tempwall.length*worldtotex)+tempwall.midmod;
			while(midtx<0) 
			{ 
				midtx+=(*(textures+midtex))->width; 
				tempwall.midmod+=(*(textures+midtex))->width; 
			}
			while(midtx>=(*(textures+midtex))->width) 
			{ 
				midtx-=(*(textures+midtex))->width; 
				tempwall.midmod-=(*(textures+midtex))->width;
			}
			
			texwall(x,h2i,h1i,midtex,midtx,h2t,h1t,miny,maxy, d*5);
			//sector->ceiling[x]=h2i;
			//sector->wall[x]=wall;
			//sector->adjtop[x]=-1;
			//return;
			keepgoing=0;
		}
		
		if(tempwall.flags==WALL_ADJOINED)
		{
			h1bi=horizon+(int)(dsd*(player.height-(sectors+tempwall.adjoin)->floorheight));
			h2bi=horizon-(int)(dsd*((sectors+tempwall.adjoin)->ceilingheight-player.height));
						
						
			if(h2bi>=miny)
			{
				if(h2bi>h2i) 
					{ 
						h2t=tempwall.topoffset;
						h2bt=tempwall.topbott;
						toptex=tempwall.toptex;
						toptx=tempwall.tophoriz+(int)(s*tempwall.length*worldtotex)+tempwall.topmod;
						
						while(toptx<0) 
						{ 
							toptx+=(*(textures+toptex))->width; 
							tempwall.topmod+=(*(textures+toptex))->width; 
						}
						while(toptx>=(*(textures+toptex))->width) 
						{	
							toptx-=(*(textures+toptex))->width; 
							tempwall.topmod-=(*(textures+toptex))->width;
						}
						texwall(x,h2i,h2bi,toptex,toptx,h2t,h2bt,miny,maxy,d*5);
						
						//sector->adjtop[x]=h2bi; 
					}
				//else sector->adjtop[x]=h2i; 
			}
			else 
			{
				h2bi=miny;
				//sector->adjtop[x]=h2bi;
			}

			if(h1bi<=maxy)
			{
				if(h1bi<h1i) 
				{ 
					h1bt=tempwall.bottomoffset;
					h1t=tempwall.bottombott;
					botttex=tempwall.botttex;
					botttx=tempwall.botthoriz+(int)(s*tempwall.length*worldtotex)+tempwall.bottmod;
					while(botttx<0) 
					{ 
						botttx+=(*(textures+botttex))->width; 
						tempwall.bottmod+=(*(textures+botttex))->width; 
					}
					while(botttx>=(*(textures+botttex))->width) 
					{ 
						botttx-=(*(textures+botttex))->width; 
						tempwall.bottmod-=(*(textures+botttex))->width;
					}
					
					texwall(x,h1bi,h1i,botttex,botttx,h1bt,h1t,miny,maxy, d*5);
				}	
			}
			else 
			{
				h1bi=maxy;
			}
	
			//sector->ceiling[x]=h2i;
			//sector->wall[x]=wall;
			if(h1bi>h1i) h1bi=h1i;
			if(h2bi<h2i) h2bi=h2i;
			//if(h2bi<maxy && h1bi>miny) DrawWallSlice(x,sectors+tempwall.adjoin,tempwall.mirror,h2bi,h1bi);
			//return;
			if(h2bi>=maxy || h1bi<=miny) keepgoing=0;
			sector=sectors+tempwall.adjoin;
			ignore=tempwall.mirror;
			miny=h2bi;
			maxy=h1bi;
		}
		
	
	}
}