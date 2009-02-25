#include "global.h"
#include "level.h"
#include <math.h>

char *GetFileLine(char *input, int *start, int size)
{
	int i;
	char *newdata;
	i=*start;
	while(input[i]!='\n' && input[i]!='\r' && i<size) i++;
	i++;
	newdata=(char*)malloc(i-*start);
	memcpy(newdata,input+*start,i-*start-1);
	memcpy(newdata+i-*start-1,"\0",1);
	*start=i+1;
	if(*start>size) *start=size;
	return newdata;
}

char *RemoveComments(char *input)
{
	int i=0;
	char *newdata;
	while(input[i]!='\0' && input[i]!='#') i++;
	newdata=(char*)malloc(i+1);
	memcpy(newdata,input,i);
	memcpy(newdata+i,"\0",1);
	free(input);
	return newdata;
}

char *GetValue(char *input, int *start, int *error)
{
	int i=*start;
	int i2;
	char *newdata;
	*error=0;
	if(input[i]=='\0')
	{
		*error=1;
		return 0;
	}
	if(input[i]==' ') 
	{
		while(input[i]==' ' && input[i]!='\0') i++;
		if(input[i]=='\0') 
		{
			*error=1;
			return 0;
		}
	}
	i2=i;
	while(input[i2]!=' ' && input[i2]!='\0') i2++;
	newdata=(char*)malloc(i2-i+1);
	memcpy(newdata,input+i,i2-i);
	memcpy(newdata+i2-i,"\0",1);
	*start=i2;
	return newdata;
}

char *GetNonEmptyLine(char *input, int *start,int size, int *error)
{
	char *newdata;
	
	*error=0;
	while(1)
	{
	    if(*start==size)
		{
			*error=1;
			return 0;
		}
		newdata=GetFileLine(input,start,size);
		newdata=RemoveComments(newdata);
		if(newdata[0]!='\0') break;
		free(newdata);
	}
	return newdata;
}
void LoadLevel()
{
	HANDLE levelfile;
	char *data;
	int size;
	int pos=0;
	int pos2=0;
	char *line;
	char *value;
	DWORD dummy;
	int error;
	int i,j;

	levelfile=CreateFile("level.dat",GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	size=GetFileSize(levelfile,NULL);
	data=(char*)malloc(size);
	ReadFile(levelfile,data,size,&dummy,NULL);
	CloseHandle(levelfile);

	line=GetNonEmptyLine(data,&pos,size,&error);
	pos2=0;
	value=GetValue(line,&pos2,&error);
	numtextures=atoi(value);
	free(value);
	free(line);
	textures=(struct Texture**)malloc(sizeof(struct Texture*)*numtextures);
	
	for(i=0;i<numtextures;i++)
	{
		line=GetNonEmptyLine(data,&pos,size,&error);
		pos2=0;
		
		value=GetValue(line,&pos2,&error);
		free(value);
		
		value=GetValue(line,&pos2,&error);
		textures[i]=LoadBMP(value,32,32);
		free(value);
		
		free(line);
	}

	line=GetNonEmptyLine(data,&pos,size,&error);
	pos2=0;
	
	value=GetValue(line,&pos2,&error);
	numsectors=atoi(value);
	free(value);
	
	free(line);

	sectors=(struct Sector*)malloc(sizeof(struct Sector)*numsectors);

	for(i=0;i<numsectors;i++)
	{
		sectors[i].lastwall=-1;
	
		line=GetNonEmptyLine(data,&pos,size,&error);
		pos2=0;
	
		value=GetValue(line,&pos2,&error);
		free(value);
		
		value=GetValue(line,&pos2,&error);
		sectors[i].ceilingheight=atof(value);
		free(value);
		
		value=GetValue(line,&pos2,&error);
		sectors[i].floorheight=atof(value);
		free(value);
		
		value=GetValue(line,&pos2,&error);
		sectors[i].numwalls=atoi(value);
		free(value);
		
		value=GetValue(line,&pos2,&error);
		sectors[i].ceiltex=textures[atoi(value)];
		free(value);
			
		value=GetValue(line,&pos2,&error);
		sectors[i].floortex=textures[atoi(value)];
		free(value);
		
		free(line);
		sectors[i].walls=(struct Wall*)malloc(sizeof(struct Wall)*sectors[i].numwalls);

		for(j=0;j<sectors[i].numwalls;j++)
		{
			int v;

			line=GetNonEmptyLine(data,&pos,size,&error);
			pos2=0;

			value=GetValue(line,&pos2,&error);
			free(value);

			value=GetValue(line,&pos2,&error);
			if(strcmp(value,"NORMAL")==0) sectors[i].walls[j].flags=WALL_NORMAL;
			if(strcmp(value,"ADJOINED")==0) sectors[i].walls[j].flags=WALL_ADJOINED;
			free(value);

			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].start.x=atof(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].start.y=atof(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].end.x=atof(value);
			free(value);

			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].end.y=atof(value);
			free(value);

			value=GetValue(line,&pos2,&error);
			v = atoi(value);
			if(v == -1) sectors[i].walls[j].adjoin = NULL;
			else		sectors[i].walls[j].adjoin = &sectors[v];
			free(value);

			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].mirror=atoi(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			v = atoi(value);
			if(v == -1) sectors[i].walls[j].toptex = NULL;
			else		sectors[i].walls[j].toptex = textures[v];
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].tophoriz=atoi(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].topoffset=atoi(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			v = atoi(value);
			if(v == -1) sectors[i].walls[j].midtex = NULL;
			else		sectors[i].walls[j].midtex = textures[v];
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].midhoriz=atoi(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].midoffset=atoi(value);
			free(value);

			value=GetValue(line,&pos2,&error);
			v = atoi(value);
			if(v == -1) sectors[i].walls[j].botttex = NULL;
			else		sectors[i].walls[j].botttex = textures[v];
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].botthoriz=atoi(value);
			free(value);
		
			value=GetValue(line,&pos2,&error);
			sectors[i].walls[j].bottomoffset=atoi(value);
			free(value);

			free(line);
		}

	}

	DoWallStuff();

	activesectors=(int*)malloc(sizeof(int)*numsectors);

	player.x=-10;
	player.y=-10;
	player.height=6;
	player.fromfloor=6;
	player.sector=&sectors[0];
	player.angle=1;

	mapinfo.rotate=0;
	mapinfo.zoom=10;
	mapinfo.show=0;
}

void DoWallStuff()
{
	struct Sector *temp;
	int i;
	int sector;
	double x1, y1, x2, y2;
	for(sector=0; sector<numsectors; sector++)
	{
		temp = &sectors[sector];
//		for(i=0;i<VSIZE;i++) temp->floor[i]=-1;
//		for(i=0;i<VSIZE;i++) temp->ceil[i]=-1;
		temp->beenin = 0;
		for(i=0; i<temp->numwalls; i++)
		{
			if(temp->walls[i].flags == WALL_ADJOINED) 
				temp->walls[i].topbott = temp->walls[i].topoffset + (temp->ceilingheight - temp->walls[i].adjoin->ceilingheight) * worldtotex;
			temp->walls[i].midbott = temp->walls[i].midoffset + (temp->ceilingheight - temp->floorheight) * worldtotex;
			if(temp->walls[i].flags == WALL_ADJOINED)
				temp->walls[i].bottombott = temp->walls[i].bottomoffset + (temp->walls[i].adjoin->floorheight - temp->floorheight) * worldtotex;

			x1 = temp->walls[i].start.x;			
			y1 = temp->walls[i].start.y;			
			x2 = temp->walls[i].end.x;			
			y2 = temp->walls[i].end.y;
			temp->walls[i].length = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
			temp->walls[i].soffset = 2 / temp->walls[i].length;
			temp->walls[i].topmod = 0;
			temp->walls[i].midmod = 0;
			temp->walls[i].bottmod = 0;
		}
	}
}