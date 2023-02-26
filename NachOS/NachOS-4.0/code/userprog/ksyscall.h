/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysReadNum()
{
  long long result = 0;
  int count = 0;
  char temp;
  char res = kernel->synchConsoleIn->GetChar();
  while (res != '\n' && res != '\r')
  {
    if (count == 0)
      temp = res;
    if (res < '0' || res > '9')
    {
      if (!(res == '-' && count == 0))
      {
        DEBUG(dbgSys, "ERROR: Input string is not numeric\n");
        while (res != '\n' && res != '\r')
          res = kernel->synchConsoleIn->GetChar();
        return 0;
      }
    }
    else
      result = result * 10 + (res - '0');
    count++;
    res = kernel->synchConsoleIn->GetChar();
  }
  if (result > 21474836478 || (result == 21474836478 && temp != '-'))
  {
    DEBUG(dbgSys, "ERROR: Size of number exceeds the limit of type int\n");
    return 0;
  }
  if (temp == '-')
    result = -result;
  return result;
}

int countDigit(int num)
{
  if (num == 0)
    return 1;
  int temp = num;
  int count = 0;
  while (temp != 0)
  {
    count++;
    temp = temp / 10;
  }
  return count;
}

int pow(int num, int ex)
{
  int result = 1;
  for (int i = 0; i < ex; i++)
  {
    result *= num;
  }
  return result;
}

void SysPrintNum(int num)
{
  int convert = num;
  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
    convert = -convert;
  }
  int count = countDigit(convert);
  while (count > 0)
  {
    int temp = convert / (int)pow(10, count - 1);
    kernel->synchConsoleOut->PutChar((char)(temp + 48));
    if (convert != 0)
    {
      convert = convert % (int)(pow(10, count - 1));
    }
    count--;
  }
}

void SysPrintChar(char ch)
{
  kernel->synchConsoleOut->PutChar(ch);
}

char SysReadChar()
{
  char result = kernel->synchConsoleIn->GetChar();
  return result;
}

int SysRandomNum()
{
  int num;
  num = 1 + rand() % RAND_MAX;
  return num;
}

int SysReadString(char *input, int length)
{
  int count = 0;
  memset(input,0,length);
  while (count < length)
  {
    char ch = kernel->synchConsoleIn->GetChar();
    if (ch == 0 || ch == '\n')
      break;
    input[count] = ch;
    count++;
  }
  return count;
}

void SysPrintString(char *output)
{
  int count = 0;
  while (output[count] != 0)
  {
    kernel->synchConsoleOut->PutChar(output[count]);
    count++;
  }
}

OpenFileId SysOpen(char *fileName)
{
  int index = -1;
  for (int i = 2; i < 10; i++)
    if (kernel->files[i] == NULL)
    {
      index = i;
      break;
    }
  if (index == -1)
    return -2;
  OpenFile *file = kernel->fileSystem->Open(fileName);
  if (file == NULL)
    return -1;
  kernel->files[index] = file;
  return index;
}

int SysClose(OpenFileId index)
{
  if (index < 2 || index > 9)
    return -1;
  if (kernel->files[index])
  {
    delete kernel->files[index];
    kernel->files[index] = NULL;
    return 0;
  }
  return -1;
}

int SysWrite(char *buffer, int length, OpenFileId index)
{
  if (index == 0)
  {
    for (int i = 0; i < length; i++)
      kernel->synchConsoleOut->PutChar(buffer[i]);
    return length;
  }
  if (index < 2 || index > 9)
    return -1;
  if (kernel->files[index] == NULL)
    return -1;
  return kernel->files[index]->Write(buffer, length);
}

int SysRead(char *buffer, int length, OpenFileId index)
{
  if (index == 1)
  {
    for (int i = 0; i < length; i++)
      buffer[i] = kernel->synchConsoleIn->GetChar();
    return length;
  }
  if (index < 2 || index > 9)
    return -1;
  if (kernel->files[index] == NULL)
    return -1;
  return kernel->files[index]->Read(buffer, length);
}

int SysRemove(char *fileName)
{
  OpenFile *file = kernel->fileSystem->Open(fileName);
  if (file == NULL)
    return -1;
  for (int i = 2; i < 10; i++)
    if (file == kernel->files[i])
      return -1;
  kernel->fileSystem->Remove(fileName);
  return 0;
}

int SysSeek(int pos, int index)
{
  if (index < 2 || index > 9)
    return -1;
  OpenFile *file = kernel->files[index];
  if (file == NULL)
    return -1;
  if (pos == -1)
    pos = file->Length();
  if (pos < 0 || pos > file->Length())
    return -1;
  file->Seek(pos);
  return pos;
}

void IncreasePC() /* Modify return point */
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
