#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	SpaceId id_file;
	char buffer[MAX_LENGTH]; 
	id_file = Open("taofile.txt", 0);

	Write("kytuduocviet", 12, id_file);
	
	Close(id_file);

	id_file = Open("taofile.txt", 1);

	
	Read(buffer, 12, id_file);
	
	PrintString(buffer);
	
	Close(id_file);
	return;
}


