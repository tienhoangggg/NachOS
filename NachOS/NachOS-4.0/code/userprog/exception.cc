// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "synchconsole.h"
#include "syscall.h"
#include "ksyscall.h"
#define MaxFileLength 32
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
//
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	kernel->machine->WriteMem(virtAddr + i, 1, 0);
	return i;
}
//
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;
	case PageFaultException:
	{
		DEBUG(dbgSys, "No valid translation found.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case ReadOnlyException:
	{
		DEBUG(dbgSys, "Write attempted to page marked read-only.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case BusErrorException:
	{
		DEBUG(dbgSys, "Translation resulted in an invalid physical address.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case AddressErrorException:
	{
		DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case OverflowException:
	{
		DEBUG(dbgSys, "Integer overflow in add or sub..\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case IllegalInstrException:
	{
		DEBUG(dbgSys, "Unimplemented or reserved instr.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case NumExceptionTypes:
	{
		DEBUG(dbgSys, "Number exception types.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	}
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		}
		case SC_Create:
		{
			DEBUG(dbgSys, "\n SC_Create call ...");
			DEBUG(dbgSys, "\n Reading virtual address of filename");
			int virtAddr = kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "\n Reading filename.");
			char *filename = User2System(virtAddr, MaxFileLength + 1); // MaxFileLength là = 32
			if (filename == NULL)
			{
				printf("\n Not enough memory in system\n");
				DEBUG(dbgSys, "\n Not enough memory in system\n");
				kernel->machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng
				delete filename;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}
			DEBUG(dbgSys, "\n Finish reading filename.");
			bool check1 = false, check2 = true;
			for (unsigned long i = 0; i < sizeof(filename); i++)
			{
				if (isalpha(filename[i]))
					check1 = true;
				if (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':' || filename[i] == '*' || filename[i] == '?' || filename[i] == '"' || filename[i] == '<' || filename[i] == '>' || filename[i] == '|')
					check2 = false;
			}
			if (!check1 || !check2)
			{
				printf("\n Wrong syntax of name\n");
				DEBUG(dbgSys, "\n Wrong syntax of name\n");
				delete filename;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}
			OpenFile *file = kernel->fileSystem->Open(filename);
			if (file != NULL)
			{
				printf("\n existed file\n");
				DEBUG(dbgSys, "\n existed file\n");
				delete filename;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}
			if (!kernel->fileSystem->Create(filename))
			{
				printf("\n Error create file '%s'", filename);
				kernel->machine->WriteRegister(2, -1);
				delete filename;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}
			kernel->machine->WriteRegister(2, 0); // trả về cho chương trình người dùng thành công
			delete filename;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Remove:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *fileName = User2System(virtAddr, MaxFileLength + 1);
			int result = SysRemove(fileName);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			kernel->machine->WriteRegister(2, result);
			delete fileName;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadNum:
		{
			int result = SysReadNum();
			DEBUG(dbgSys, "ReadNum returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintNum:
		{
			DEBUG(dbgSys, "PrintNum " << kernel->machine->ReadRegister(4) << "\n");
			int num = kernel->machine->ReadRegister(4);
			SysPrintNum(num);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintChar:
		{
			DEBUG(dbgSys, "Char is " << (char)kernel->machine->ReadRegister(4) << "\n");
			char ch = kernel->machine->ReadRegister(4);
			SysPrintChar(ch);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadChar:
		{
			char ch;
			ch = SysReadChar();
			if (ch != '\0')
			{
				DEBUG(dbgSys, "ReadChar returning with " << ch << "\n");
			}
			else
			{
				DEBUG(dbgSys, "ReadChar returning with \\0" << ch << "\n");
			}
			kernel->machine->WriteRegister(2, (char)ch);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_RandomNum:
		{
			srand(time(NULL));
			int random = SysRandomNum();
			DEBUG(dbgSys, "RandomNum returning with " << random << "\n");
			kernel->machine->WriteRegister(2, (int)random);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadString:
		{
			int address = kernel->machine->ReadRegister(4);
			int limit = kernel->machine->ReadRegister(5);
			char *buff = new char[limit];
			int countR = SysReadString(buff, limit);
			int countS2U = System2User(address, countR, buff);
			DEBUG(dbgSys, "Number of entered characters " << countR << "\n");
			DEBUG(dbgSys, "Number of System2User " << countS2U << "\n");
			delete buff;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintString:
		{
			int address = kernel->machine->ReadRegister(4);
			char *str = User2System(address, 255);
			SysPrintString(str);
			delete str;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Open:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *fileName = User2System(virtAddr, MaxFileLength + 1);
			int result = SysOpen(fileName);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			kernel->machine->WriteRegister(2, result);
			delete fileName;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Close:
		{
			int index = kernel->machine->ReadRegister(4);
			int result = SysClose(index);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Write:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			int limit = kernel->machine->ReadRegister(5);
			int index = kernel->machine->ReadRegister(6);
			char *buffer = User2System(virtAddr, 255);
			int result = SysWrite(buffer, limit, index);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			kernel->machine->WriteRegister(2, result);
			delete buffer;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Read:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			int limit = kernel->machine->ReadRegister(5);
			int index = kernel->machine->ReadRegister(6);
			char *buffer = User2System(virtAddr, 255);
			int result = SysRead(buffer, limit, index);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			System2User(virtAddr, result, buffer);
			kernel->machine->WriteRegister(2, result);
			delete buffer;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Seek:
		{
			int pos = kernel->machine->ReadRegister(4);
			int index = kernel->machine->ReadRegister(5);
			int result = SysSeek(pos, index);
			if (result < 0)
			{
				DEBUG(dbgSys, "\nError!");
			}
			kernel->machine->WriteRegister(2, result);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
			break;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}