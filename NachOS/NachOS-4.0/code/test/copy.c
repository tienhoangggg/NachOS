#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    int openFileId;
    int fileSize;
    char fileName[MAX_LENGTH];
    char *content;
    PrintString("$copy ");
    ReadString(fileName, MAX_LENGTH);
    openFileId = Open(fileName);
    if(openFileId<0)
    {
        Halt();
    }
    fileSize = Seek(-1, openFileId);
    Seek(0, openFileId);
    fileSize = Read(content, fileSize, openFileId);
    Close(openFileId);
    ReadString(fileName, MAX_LENGTH);
    Create(fileName);
    openFileId = Open(fileName);
    if (Write(content, fileSize, openFileId) < 0)
    {
        PrintString("Can't write file!\n");
        Close(openFileId);
        Remove(fileName);
        Halt();
    }
    Close(openFileId);
    Halt();
}