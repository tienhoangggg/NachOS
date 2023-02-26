#include "syscall.h"
#define maxlen 32
int
main()
{
 char filename[maxlen];
 PrintString("$create ");
 ReadString(filename, maxlen);
 Create(filename);
 Halt();
} 