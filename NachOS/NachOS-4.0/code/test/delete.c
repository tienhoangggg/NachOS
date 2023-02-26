#include "syscall.h"
#define maxlen 32
int
main()
{
 char *filename;
 PrintString("$delete ");
 ReadString(filename, maxlen);
 if (Remove(filename) < 0)
 {
     PrintString("Error!\n");
 }
 else
 {
     PrintString("Done!\n");
 }
 Halt();
} 