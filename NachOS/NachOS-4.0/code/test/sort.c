#include "syscall.h"

void BubbleSort(int a[], int n, int choose)
{
    int i, j, temp;
    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            if ((a[i] < a[j] && choose == 2) || (a[i] > a[j] && choose == 1))
            {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
}
int main()
{
    int n, i, choose;
    int a[100];
    do
    {
        PrintString("Input n: ");
        n = ReadNum();
        if (n > 100 || n < 1)
            PrintString("n must be less than 100\n");
    } while (n > 100 || n < 1);
    for (i = 0; i < n; i++)
    {
        PrintString("array[");
        PrintNum(i);
        PrintString("] = ");
        a[i] = ReadNum();
    }
    PrintString("1: Increase\n");
    PrintString("2: Decrease\n");
    do
    {
        PrintString("Choose sort order you want: ");
        choose = ReadNum();
        if (choose != 1 && choose != 2)
            PrintString("ERROR: Just choose 1 or 2.\n");
    } while (choose != 1 && choose != 2);
    BubbleSort(a, n, choose);
    for (i = 0; i < n; i++)
    {
        PrintNum(a[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
    Halt();
}