```
#include "includes.h"
#include "uart.h"
#include "lcd.h"

OS_STK Task1Stack[100];
OS_STK Task2Stack[100];
OS_STK Task3Stack[100];

void Task1(void *pdata);
void Task2(void *pdata);
void Task3(void *pdata);

int main(void)
{
    timer_init();
    OSInit();

    // Create Task 1
    // Syntax: OSTaskCreate(TaskFunction, TaskDataPointer, TaskStackPointer, TaskPriority)
    // TaskFunction: Name of the task function to be executed
    // TaskDataPointer: Pointer to task-specific data (can be NULL if not used)
    // TaskStackPointer: Pointer to the task's stack memory
    // TaskPriority: Priority level of the task (lower value indicates higher priority)
    OSTaskCreate(Task1, (void *)0, &Task1Stack[99], 1);
    OSTaskCreate(Task2, (void *)0, &Task2Stack[99], 2);
    OSTaskCreate(Task3, (void *)0, &Task3Stack[99], 3);

    OSStart();

    // return 0; (Uncomment this line if needed)
}

void Task1(void *pdata)
{
    int i = 0;

    Lcd_Init();             // Initialize LCD in 8-bit mode
    Lcd_Cmd(0x80);          // Set LCD cursor to line 1
    Lcd_String("numbers");

    while (1)
    {
        Lcd_Cmd(0x88);      // Set LCD cursor to position 0x88 (line 1, column 9)
        Lcd_Data(0x30 + i); // Display the number on the LCD

        i++;                // Increment the number

        if (i == 10)
            i = 0;          // Reset the number to 0 when it reaches 10

        OSTimeDlyHMSM(0, 0, 1, 0); // Delay for 1 second
    }
}

void Task2(void *pdata)
{
    int i = 0;

    Lcd_Cmd(0xC0);          // Set LCD cursor to line 2
    Lcd_String("alphabets");

    while (1)
    {
        Lcd_Cmd(0xCB);      // Set LCD cursor to position 0xCB (line 2, column 12)
        Lcd_Data(0x41 + i); // Display the alphabet on the LCD

        i++;                // Increment the alphabet

        if (i == 26)
            i = 0;          // Reset the alphabet to 'A' when it reaches 'Z'

        OSTimeDlyHMSM(0, 0, 0, 500); // Delay for 500 milliseconds
    }
}

void Task3(void *pdata)
{
    int i = 0;

    Uart0_Init(4800);       // Initialize UART0 with baud rate 4800

    while (1)
    {
        uprintf("\x1b[1;1HTask3 %d04", ii++); // Send formatted string to UART0
        // \x1b[1;1H --> the ANSI escape sequence for setting the cursor position on the console
        // moves the cursor to row 1 (line 1) and column 1 (leftmost column) on the console

        i++;

        if (i == 9999)
            i = 0;          // Reset the counter to 0 when it reaches 9999

        OSTimeDlyHMSM(0, 0, 1, 0); // Delay for 1 second
    }
}

```