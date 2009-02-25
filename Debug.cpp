#include "global.h"
#include "debug.h"

void DebugString(char *buffer)
{
	DWORD dummy;

	//if(KEY_DOWN(VK_F2)) 
		WriteFile(DebugFile,buffer,strlen(buffer),&dummy,NULL);
}

void WriteStruct(void *data, int size)
{
	int i;
	char *string;
	char buffer[50];

	string=(char*)data;

	wsprintf(buffer,"Size: %i",size);
	DebugString(buffer);
	for(i=0;i<size;i++)
	{
		wsprintf(buffer,"%x ",*(string+i));
		DebugString(buffer);
	}
}