#include "level.h"
#include <math.h>

#include "World.h"
#include "Texture.h"

char *GetFileLine(char *input, int *start, int size)
{
	int i;
	char *newdata;
	i = *start;
	while(input[i]!='\n' && input[i]!='\r' && i<size) i++;
	i++;
	newdata = new char[i-*start];
	memcpy(newdata, input + *start, i - *start - 1);
	memcpy(newdata + i - *start - 1, "\0", 1);
	*start = i + 1;
	if(*start > size) *start = size;
	return newdata;
}

char *RemoveComments(char *input)
{
	int i=0;
	char *newdata;
	while(input[i]!='\0' && input[i]!='#') i++;
	newdata = new char[i+1];
	memcpy(newdata, input, i);
	memcpy(newdata + i, "\0", 1);
	delete[] input;

	return newdata;
}

char *GetValue(char *input, int *start, bool *error)
{
	int i = *start;
	int i2;
	char *newdata;
	*error = false;
	if(input[i] == '\0')
	{
		*error = true;
		return 0;
	}
	if(input[i] == ' ') 
	{
		while(input[i]==' ' && input[i]!='\0') i++;
		if(input[i] == '\0') 
		{
			*error = true;
			return 0;
		}
	}
	i2 = i;
	while(input[i2]!=' ' && input[i2]!='\0') i2++;
	newdata = new char[i2-i+1];
	memcpy(newdata, input + i, i2 - i);
	memcpy(newdata + i2 - i, "\0", 1);
	*start = i2;
	return newdata;
}

char *GetNonEmptyLine(char *input, int *start,int size, bool *error)
{
	char *newdata;
	
	*error = 0;
	while(1)
	{
	    if(*start == size)
		{
			*error = true;
			return 0;
		}
		newdata = GetFileLine(input, start, size);
		newdata = RemoveComments(newdata);
		if(newdata[0] != '\0') break;
		delete[] newdata;
	}
	return newdata;
}

static void DoWallStuff(Level *level)
{
	struct Sector *temp;
	int i;
	int sector;
	double x1, y1, x2, y2;
	for(sector=0; sector<level->numSectors; sector++)
	{
		temp = &level->sectors[sector];
//		for(i=0;i<VSIZE;i++) temp->floor[i]=-1;
//		for(i=0;i<VSIZE;i++) temp->ceil[i]=-1;
		temp->beenin = 0;
		for(i=0; i<temp->numWalls; i++)
		{
			if(temp->walls[i].flags == WALL_ADJOINED) 
				temp->walls[i].topbott = temp->walls[i].topoffset + (temp->ceilingheight - temp->walls[i].adjoin->ceilingheight) * WORLDTOTEX;
			temp->walls[i].midbott = temp->walls[i].midoffset + (temp->ceilingheight - temp->floorheight) * WORLDTOTEX;
			if(temp->walls[i].flags == WALL_ADJOINED)
				temp->walls[i].bottombott = temp->walls[i].bottomoffset + (temp->walls[i].adjoin->floorheight - temp->floorheight) * WORLDTOTEX;

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

Level *LoadLevel()
{
	HANDLE levelfile;
	char *data;
	int size;
	int pos=0;
	int pos2=0;
	char *line;
	char *value;
	DWORD dummy;
	bool error;
	int i,j;

	Level *level = new Level;

	levelfile = CreateFile("level.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	size = GetFileSize(levelfile, NULL);
	data = new char[size];
	ReadFile(levelfile, data, size, &dummy, NULL);
	CloseHandle(levelfile);

	line = GetNonEmptyLine(data, &pos, size, &error);
	pos2 = 0;
	value = GetValue(line, &pos2, &error);
	level->numTextures = atoi(value);
	delete[] value;
	delete[] line;
	level->textures = new Texture[level->numTextures];
	
	for(i=0; i<level->numTextures; i++)
	{
		line = GetNonEmptyLine(data, &pos, size, &error);
		pos2 = 0;
		
		value = GetValue(line, &pos2, &error);
		delete[] value;
		
		value = GetValue(line, &pos2, &error);
		level->textures[i] = LoadBMP(value, 32, 32);
		delete[] value;
		
		delete[] line;
	}

	line = GetNonEmptyLine(data, &pos, size, &error);
	pos2 = 0;
	
	value = GetValue(line, &pos2, &error);
	level->numSectors = atoi(value);
	delete[] value;
	
	delete[] line;

	level->sectors = new Sector[level->numSectors];

	for(i=0; i<level->numSectors; i++)
	{
		level->sectors[i].lastwall = -1;
	
		line = GetNonEmptyLine(data, &pos, size, &error);
		pos2 = 0;
	
		value = GetValue(line, &pos2, &error);
		delete[] value;
		
		value = GetValue(line, &pos2, &error);
		level->sectors[i].ceilingheight = atof(value);
		delete[] value;
		
		value = GetValue(line, &pos2, &error);
		level->sectors[i].floorheight = atof(value);
		delete[] value;
		
		value = GetValue(line, &pos2, &error);
		level->sectors[i].numWalls = atoi(value);
		delete[] value;
		
		value = GetValue(line, &pos2, &error);
		level->sectors[i].ceiltex = &level->textures[atoi(value)];
		delete[] value;
			
		value = GetValue(line, &pos2, &error);
		level->sectors[i].floortex = &level->textures[atoi(value)];
		delete[] value;
		
		delete[] line;

		level->sectors[i].walls = new Wall[level->sectors[i].numWalls];

		for(j=0; j<level->sectors[i].numWalls; j++)
		{
			int v;
			Sector *sector = &level->sectors[i];

			line = GetNonEmptyLine(data, &pos, size, &error);
			pos2 = 0;

			value = GetValue(line, &pos2, &error);
			delete[] value;

			value = GetValue(line, &pos2, &error);
			if(strcmp(value, "NORMAL") == 0) sector->walls[j].flags = WALL_NORMAL;
			if(strcmp(value, "ADJOINED") == 0) sector->walls[j].flags = WALL_ADJOINED;
			delete[] value;

			value = GetValue(line, &pos2, &error);
			sector->walls[j].start.x = atof(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].start.y = atof(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].end.x = atof(value);
			delete[] value;

			value = GetValue(line, &pos2, &error);
			sector->walls[j].end.y = atof(value);
			delete[] value;

			value = GetValue(line, &pos2, &error);
			v = atoi(value);
			if(v == -1) sector->walls[j].adjoin = NULL;
			else		sector->walls[j].adjoin = &level->sectors[v];
			delete[] value;

			value = GetValue(line, &pos2, &error);
			sector->walls[j].mirror = atoi(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			v = atoi(value);
			if(v == -1) sector->walls[j].toptex = NULL;
			else		sector->walls[j].toptex = &level->textures[v];
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].tophoriz = atoi(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].topoffset = atoi(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			v = atoi(value);
			if(v == -1) sector->walls[j].midtex = NULL;
			else		sector->walls[j].midtex = &level->textures[v];
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].midhoriz = atoi(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].midoffset = atoi(value);
			delete[] value;

			value = GetValue(line, &pos2, &error);
			v = atoi(value);
			if(v == -1) sector->walls[j].botttex = NULL;
			else		sector->walls[j].botttex = &level->textures[v];
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].botthoriz = atoi(value);
			delete[] value;
		
			value = GetValue(line, &pos2, &error);
			sector->walls[j].bottomoffset = atoi(value);
			delete[] value;

			delete[] line;
		}

	}

	DoWallStuff(level);

	level->playerStart.x = -10;
	level->playerStart.y = -10;
	level->playerStart.height = 6;
	level->playerStart.fromfloor = 6;
	level->playerStart.sector = &level->sectors[0];
	level->playerStart.angle = 1;

	return level;
}

