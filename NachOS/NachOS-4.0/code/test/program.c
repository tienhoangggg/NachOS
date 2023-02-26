#include "syscall.h"

void mainHelp()
{
    PrintString("\t---GROUP INFOMATION---\t\n");
    PrintString("Nguyen Nhat Cuong - 19127346 \n");
    PrintString("Vo Tran Quang Thong - 19127567\n");
    PrintString("Dinh Hai Giang - 19127382\n\n");

    PrintString("\t---BUBBLE SORT---\t\n");
    PrintString("Bubble sort, sometimes referred to as sinking sort\n");
    PrintString("It is a simple sorting algorithm that repeatedly steps through the list\n");
    PrintString("It compares adjacent elements and swaps them if they are in the wrong order\n");
    PrintString("The pass through the list is repeated until the list is sorted.\n\n");

    PrintString("\t---ASCII---\t\n");
    PrintString("Stands for: American Standard Code for Information Interchange.\n");
    PrintString("ASCII is a character encoding that uses numeric codes to represent characters.\n");
    PrintString("These include upper and lowercase English letters, numbers, and punctuation symbols\n\n");
}

void mainASCII()
{
    int i = 0;
    PrintString("\t\t------ASCII------\t\t\n");
    for (i = 33; i < 65; i++)
    {
        PrintNum(i);
        PrintChar('\t');
        PrintChar((char)i);
        PrintString("\t|");

        PrintNum(i + 32);
        PrintChar('\t');
        PrintChar((char)(i + 32));
        PrintString("\t|");

        if (i + 64 > 127)
        {
            PrintChar('\t');
            PrintString("\t|\n");
            break;
        }

        PrintNum(i + 64);
        PrintChar('\t');
        PrintChar((char)(i + 64));
        PrintString("\t|\n");
    }
}

void mainOutputArray(int a[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        PrintNum(a[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
}

void mainSort()
{
    int n;
    int i;
    int j;
    int temp;
    int choose;
    int a[101];

    do
    {
        PrintString("Input n: ");
        n = ReadNum();

        if (n > 100 || n < 1)
        {
            PrintString("ERROR: n <= 100\n");
        }
    } while (n > 100 || n < 1);

    for (i = 0; i < n; i++)
    {
        PrintString("a[");
        PrintNum(i);
        PrintString("] = ");
        a[i] = ReadNum();
    }

    PrintString("1: Decrease\n");
    PrintString("2: Increase\n");

    do
    {
        PrintString("Choose sort order you want: ");
        choose = ReadNum();

        if (choose < 1 || choose > 2)
        {
            PrintString("ERROR: Just choose 1 or 2.\n");
        }
    } while (choose < 1 || choose > 2);

    if (choose == 1)
    {
        // Bubble sort for Descending
        for (i = 0; i < n - 1; i++)
        {
            for (j = 0; j < n - i - 1; j++)
            {
                if (a[j] < a[j + 1])
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
            }
        }
    }
    else if (choose == 2)
    {
        // Bubble sort for Ascending
        for (i = 0; i < n - 1; i++)
        {
            int j;
            for (j = 0; j < n - i - 1; j++)
            {
                if (a[j] > a[j + 1])
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
            }
        }
    }

    mainOutputArray(a, n);
}

int main()
{
    mainHelp();
    mainASCII();
    mainSort();

    Halt();
}