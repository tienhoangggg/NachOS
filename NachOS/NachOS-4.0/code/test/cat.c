#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int openFileId;
	int fileSize;
	char fileName[MAX_LENGTH];
	char *content;
	PrintString("$cat ");
	ReadString(fileName, MAX_LENGTH);
	openFileId = Open(fileName);
	if(openFileId<0)
	{
		Halt();
	}
	fileSize = Seek(-1, openFileId);
	Seek(0, openFileId);
	if (Read(content, fileSize, openFileId) > 0)
		PrintString(content);
	PrintChar('\n');
	Close(openFileId);
	Halt();
}