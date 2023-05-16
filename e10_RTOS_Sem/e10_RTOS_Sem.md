```
#include "includes.h"
#include "edutech.h"
#include "uart.h"
#include "lcd.h"

#define UART_DEBUG 0

#if UART_DEBUG
/* Debug task stack */
OS_STK UartDebugStack[100];

/* UART Debug Task */
void UART_Debug(void *pdata)
{
    OS_STK_DATA data;
    Uart0_Init(9600);
    while(1)
    {
        OSTaskStkChk(7, &data); // Provide the priority of task here
        uprintf("\x1b[1;1HTask1 %d04 %d04 %d04", data.OSFree + data.OSUsed, data.OSFree, data.OSUsed);
        // \x1b[2;1H --> the ANSI escape sequence for setting the cursor position on the console
        // moves the cursor to row 2 (line 2) and column 1 (leftmost column) on the console
        OSTimeDlyHMSM(0, 0, 0, 500);
    }
}
#endif

OS_EVENT *semaphore;

/* Task1 Stack */
OS_STK Task1Stack[100];
void Task1(void *pdata);

/* Task2 Stack */
OS_STK Task2Stack[100];
void Task2(void *pdata);

/* Main Program */
int main(void)
{
    timer_init(); // initialize OS Timer Tick
    OSInit(); // Initialize uC/OS-II

    /* Create Debug task */
#if UART_DEBUG
    OSTaskCreateExt(UART_Debug, (void *)0, &UartDebugStack[99], 7, 0, &UartDebugStack[0], 100, (void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#endif

    semaphore = OSSemCreate(1);

    OSTaskCreate(Task1, (void *)0, &Task1Stack[99], 2); // Create task1
    OSTaskCreate(Task2, (void *)0, &Task2Stack[99], 3); // Create task2

    /* start the multitasking process which lets uC/OS-II manage the tasks that you have created */
    OSStart();

    return 0;
}

/* Task Definition */

/**
 * Task1 to Print A to Z on LCD line1
 */
void Task1(void *pdata)
{
    unsigned char i = 0;
    INT8U err;
    Uart0_Init(9600); // Initialize UART0

    while (1)
    {
        OSSemPend(semaphore, 0, &err); // Wait for semaphore

        uprintf("\x1b[1;1HTask1 %c", 0x41 + i++);

        if (i == 26)
            i = 0;

        OSTimeDlyHMSM(0, 0, 0, 500); // Delay 500ms

        OSSemPost(semaphore); // Give semaphore
    }
}

/**
 * Task2 to Print 0 to 9 on LCD line2
 */
void Task2(void *pdata)
{
    int i = 0;
    INT8U err;
    Uart0_Init(9600); // Initialize UART0

    while (1)
    {
        OSSemPend(semaphore, 0, &err); // Wait for semaphore

        uprintf("\x1b[2;1HTask2 %d02", i++);

        if (i == 10)
            i = 0;

        OSTimeDlyHMSM(0, 0, 0, 500); // Delay 500ms

        OSSemPost(semaphore); // Give semaphore
    }
}

```