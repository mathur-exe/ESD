# ADC with Interrupt

![_](https://github.com/mathur-exe/ESD/blob/main/img/e4_ADC.png)

```
#include "LPC214x.h"
#include "VIClowlevel.h"

#define RS (1<<5)
#define RW (1<<6)
#define EN (1<<7)

void delay_fv(unsigned int x, int y);
void lcd_display(unsigned int x);
void cmd(unsigned int x);
void lcd_ini(void);
void lcd_str(unsigned char x[]);
// void lcd_str(unsigned char *x);
void DisplayHexLcd(int LcdData);

unsigned int AdcData = 0;

void __attribute__((interrupt("IRQ"))) ADC0_routine(void)
{
    ISR_ENTRY();
    AdcData = (AD0GDR & 0x0000FFC0) >> 6;
    DisplayHexLcd(AdcData);
    AD0INTEN = 0x00;
    VICVectAddr = 0x00;
    ISR_EXIT();
}
int main()
{
    int Result;
    PINSEL0 = 0x00000000;  // Configure the PINSEL0 register for GPIO functionality
    PINSEL1 = 0x01000000;  // Configure the PINSEL1 register for ADC functionality
    VPBDIV = 0x01;  // Set the VPBDIV register to divide the clock by 1
    
    IODIR0 = 0XFFE0;  // Configure the IODIR0 register to set certain pins as output
    
    lcd_ini();  // Initialize the LCD
    cmd(0x80);  // Set the LCD cursor to the beginning of the first line
    lcd_str("ADC o/p:");  // Display a string on the LCD
    cmd(0x8B);  // Set the LCD cursor to a specific position
    lcd_display('H');  // Display a character on the LCD
    
    while (1)
    {
        VICIntEnable |= (0x01 << 18);  // Enable the ADC interrupt in the VIC
        VICVectAddr0 = (unsigned)ADC0_routine;  // Set the address of the ADC0_routine as the ISR
        VICVectCntl0 = (0x32);  // Set the VICVectCntl0 register for ADC0 interrupt
        enableIRQ();  // Enable IRQ interrupts
        
        PINSEL1 = 0x1000000;  // Reconfigure the PINSEL1 register for ADC functionality
        VPBDIV = 0x01;  // Set the VPBDIV register to divide the clock by 1
        AD0CR = 0X01200402;  // Configure the AD0CR register for ADC conversion
        ADGSR = 0X01000000;  // Clear the ADGSR register
        AD0INTEN = 0x02;  // Enable ADC interrupt for channel 2
    }
    
    Result = ((AD0GDR & 0x0000FFC0) >> 6);  // Get the ADC result
    DisplayHexLcd(Result);  // Display the ADC result in hexadecimal on the LCD
    
    while (1);
}
void lcd_display(unsigned int x)
{
    IOCLR0 = 0x0000FF00;  // Clear the LCD data pins
    x = (x << 8);  // Shift the data to the correct position
    IOSET0 = x;  // Set the LCD data pins
    IOSET0 = RS;  // Set RS pin for data mode
    IOCLR0 = RW;  // Clear RW pin for write mode
    IOSET0 = EN;  // Set EN pin to enable the LCD
    delay_fv(100, 10);  // Delay for LCD operation
    IOCLR0 = EN;  // Clear EN pin to disable the LCD
}

void delay_fv(unsigned int x, int y)
{
    unsigned int i, j;
    for (i = 0; i < x; i++)
        for (j = 0; j < y; j++)
            ;
}

void cmd(unsigned int x)
{
    IOCLR0 = 0x0000FF00;  // Clear the LCD data pins
    x = (x << 8);  // Shift the data to the correct position
    IOSET0 = x;  // Set the LCD data pins
    IOCLR0 = RS;  // Clear RS pin for command mode
    IOCLR0 = RW;  // Clear RW pin for write mode
    IOSET0 = EN;  // Set EN pin to enable the LCD
    delay_fv(100, 10);  // Delay for LCD operation
    IOCLR0 = EN;  // Clear EN pin to disable the LCD
}

void lcd_ini()
{
    cmd(0X38);  // Initialize the LCD with command 0x38
    cmd(0X0E);  // Display on, cursor blinking
    cmd(0X01);  // Clear the LCD screen
    cmd(0X06);  // Set the cursor to increment mode
}

void lcd_str(unsigned char x[])
{
    int i;
    for (i = 0; x[i] != '\0'; i++)
    {
        lcd_display(x[i]);  // Display each character of the string
    }
}

void DisplayHexLcd(int LcdData)
{
    unsigned char Character[17] = "0123456789ABCDEF";  // Hexadecimal characters
    unsigned int DivValue = 0x100, BaseValue = 0x10;
    char j = 0;
    while (DivValue)
    {
        cmd(0x88 + j++);  // Set the LCD cursor position
        lcd_display(Character[LcdData / DivValue]);  // Display the hexadecimal digit
        LcdData %= DivValue;
        DivValue /= BaseValue;
    }
}
```

# UART

![_](https://github.com/mathur-exe/ESD/blob/main/img/ID_UART.png)
```
#include "lpc214x.h"
#define FOSC 12000000

void Transm(char);
char Receivem();

int main()
{
    // Configure UART
    PINSEL0 = 0X00000005;   // Select UART0 functionality for pins P0.0 (TXD0) and P0.1 (RXD0) of LPC2148
    VPBDIV = 0x01;          // Set PCLK = CCLK (divided by 1), i.e VPB now operates at CCLK freq thereby now uart also operates at same freq 
    IO0DIR = 0x01;          // Set GPIO P0.0 (TXD0) as an output pin
    U0FCR = 0x07;           // Enable FIFO (FIFO Enable = 1, RX and TX FIFO Reset = 1)
    U0LCR = 0x87;           // Enable access to Divisor Latch (DLAB = 1), set data frame to 8-bit, no parity, 1 stop bit
    U0DLL = 0x4E;           // Set Divisor Latch LSB value for baud rate generation (78 in decimal) [CHANGES WITH BAUDRATE]
    U0DLM = 0x00;           // Set Divisor Latch MSB value for baud rate generation
    U0LCR = 0x07;           // Disable access to Divisor Latch (DLAB = 0), enable UART (UART Enable = 1)
    U0TER = 0x80;           // Enable UART transmitter (Transmitter Enable = 1)

    // Transmit 'A' character
    Transm('A');

    while (1)
    {
        // Receive a character and transmit it back
        char k = Receivem();
        Transm(k);
    }

    return 0;
}

void Transm(char a)
{
    U0THR = a;                   // Transmit the character by writing it to the Transmitter Holding Register
    while (!(U0LSR & 0x20));     // Wait until the transmitter is empty (LSR bit 5 set)
        // U0LSR is the UART0 Line Status Register.
        // & is the bitwise AND operator.
        // 0x20 is a hexadecimal value for the Empty Transmitter Holding Register (THRE) status bit of U0LSR 
}

char Receivem()
{
    while (!(U0LSR & 0x01));     // Wait until a character is received (LSR bit 0 set)
    char b = U0RBR;              // Read the received character from the Receiver Buffer Register
    return b;
}
```

# GPS

![_](https://github.com/mathur-exe/ESD/blob/main/img/ID_GPS.png)

```
#include "gps.h"
#include "delay.h"
#include "lpc214x.h"
#include "uart.h"

int main()
{
    int pclk = 12000000, value;

    // Configure UART0
    PINSEL0 = 0x00050005;   // Select UART0 functionality for pins P0.0 (TXD0) and P0.1 (RXD0)
    IO0DIR = 0x101;         // Set P0.0 (TXD0) and P0.8 (TXD1) as output pins

    VPBDIV = 0x01;          // Set PCLK = CCLK (divided by 1)

    U0FCR = 0x07;           // Enable FIFO (FIFO Enable = 1, RX and TX FIFO Reset = 1)
    U0LCR = 0x83;           // Set data format of 8-bit, no parity, 1 stop bit

    U0FDR = 0xC1;           // Set fractional divider to control the clock prescaler for the baud rate generation

    value = pclk / (16 * 115200);
    U0DLL = value & 0x0FF;              // Set divisor latch LSB with the lower 8 bits of the value
    U0DLM = (value & 0xFF00) >> 8;      // Set divisor latch MSB with the upper 8 bits of the value

    U0TER = 0x80;           // Enable UART0 transmitter

    U0LCR = 0x03;           // Enable access to the UART0 divisor latches

    // Configure UART1
    U1FCR = 0x07;           // Enable FIFO (FIFO Enable = 1, RX and TX FIFO Reset = 1)
    U1LCR = 0x83;           // Set data format of 8-bit, no parity, 1 stop bit

    value = pclk / (16 * 9600);
    U1DLL = value & 0x0FF;              // Set divisor latch LSB with the lower 8 bits of the value
    U1DLM = (value & 0xFF00) >> 8;      // Set divisor latch MSB with the upper 8 bits of the value

    U1TER = 0x80;           // Enable UART1 transmitter

    U1LCR = 0x03;           // Enable access to the UART1 divisor latches

    DelayMs(500);

    while (1)
    {
        while (!(U1LSR & 0x01));    // Wait until a character is received on UART1
        char data1 = U1RBR;         // Read the received character
        U0THR = data1;              // Transmit the received character on UART0
        while (!(U0LSR & 0x20));    // Wait until the UART0 transmitter is empty
    }
}
```

# EEPROM

![_](https://github.com/mathur-exe/ESD/blob/main/img/ID_EEPROM.png)

```
#include "LPC214x.h"
#include "lcd.h"

#define PCLK 12000000

void DelayMs(unsigned int Ms);
void ConfigI2c0(int BaudRate);
void WriteEeprom0(char SlaveAddress, unsigned char *Data, char len);
void ReadEeprom0(char SlaveAddress, unsigned char *Data, char len);

unsigned char addr = 0x00;

int main(void)
{
    unsigned char EepromBufA[] = {0x00, 'M', 'A', 'Y', 'A', 'N', 'K'};
    unsigned char EepromBufB[2] = {0x00, 0};
    unsigned int No;

    edutechlcdinit();                           // Initialize the LCD

    ConfigI2c0(100);                             // Configure I2C with a baud rate of 100

    WriteEeprom0(0xA0, EepromBufA, 4);            // Write data to the EEPROM with slave address 0xA0
    edutechlcdstring("WriteSuccess", 1, 0);       // Display "WriteSuccess" message on the LCD

    for (No = 0; No < 4; No++)
    {
        ReadEeprom0(0xA0, EepromBufB, 1);         // Read data from the EEPROM with slave address 0xA0
        edutechlcdstring(&EepromBufB[0], 2, No);  // Display the read data on the LCD
        DelayMs(100);                            // Delay for 100 milliseconds
        addr++;                                  // Increment the address
    }

    while (1)
    {
    }

    return 0;
}

void DelayMs(unsigned int Ms)
{
    Ms = Ms * 200;                               // Scale the delay value
    while (Ms)                                   // Loop for the specified number of milliseconds
        --Ms;                                    // Decrement the delay counter
}

void ConfigI2c0(int BaudRate)
{
    PINSEL0 = 0x050;                             // Configure pins P0.2 and P0.3 for I2C0

    I2C0CONCLR = 0x6c;                           // Disable I2C and clear control registers
    I2C0CONSET = 0x40;                           // Enable I2C

    I2C0SCLH = (PCLK / (2 * (BaudRate * 1000))); // Set the I2C clock high period
    I2C0SCLL = (PCLK / (2 * (BaudRate * 1000))); // Set the I2C clock low period
}
unsigned char I2C_WaitStatus0(unsigned char I2CStatus)
{
    unsigned int Time, I2C_WAIT_TIME_OUT;
    I2C_WAIT_TIME_OUT = 200;
    Time = 0;

    while (Time++ < I2C_WAIT_TIME_OUT)
    {
        if (I2C0CONSET & 8)  // Poll SI bit for communication complete
        {
            if (I2C0STAT == I2CStatus)  // Read I2C status value
            {
                Time = 0;
                return 1;  // Status match, return success
            }
        }
    }

    return 0;  // Timeout, return failure
}

char WriteDataI2c0(char SlaveAddress, char *Data, unsigned char len)
{
    I2C0CONCLR = 0x2c;  // Disable I2C
    I2C0CONSET = 0x40;  // Enable I2C
    I2C0CONSET |= 0x20;

    if (!I2C_WaitStatus0(0x08))  // Wait for ready state (0x08: ready for device address)
        return 0;  // Timeout, return failure

    I2C0DAT = SlaveAddress;  // Send slave address
    I2C0CONCLR = 0x2C;

    if (!I2C_WaitStatus0(0x18))  // Wait for ready state (0x18: ready for device address)
        return 0;  // Timeout, return failure

    I2C0DAT = *Data++;  // Send data
    I2C0CONCLR = 0x2C;

    if (!I2C_WaitStatus0(0x28))  // Wait for ready state (0x28: ready for device address)
        return 0;  // Timeout, return failure

    while (len)
    {
        I2C0CONCLR = 0x2C;

        if (!I2C_WaitStatus0(0x28))  // Wait for ready state (0x28: ready for device address)
            return 0;  // Timeout, return failure

        I2C0DAT = *Data++;

        if (len > 1)
            I2C0CONSET = 0x04 | 0x40;  // Set ACK and STO bits for continuing transmission
        else
            I2C0CONSET = (0x10 | 0x40);  // Set STO bit for stop condition

        len--;
    }

    I2C0CONSET = (0x10 | 0x40);  // Send Stop bit
    I2C0CONCLR = 0x2C;

    return 1;  // Write operation successful
}

char ReadDataI2c0(char SlaveAddress, char *Data, char len)
{
    I2C0CONCLR = 0x2c;  // Disable I2C
    I2C0CONSET = 0x40;  // Enable I2C
    I2C0CONSET |= 0x20;

    if (!I2C_WaitStatus0(0x08))  // Wait for ready state (0x08: ready for device address)
        return 0;  // Timeout, return failure

    I2C0DAT = SlaveAddress;  // Send slave address
    I2C0CONCLR = 0x28;
    if (!I2C_WaitStatus0(0x40))  // Wait for ready state (0x40: ready for device address)
    return 0;  // Timeout, return failure

    while (len)
    {
        I2C0CONCLR = 0x2C;

        if (!I2C_WaitStatus0(0x58))  // Wait for ready state (0x58: ready for device address)
            return 0;  // Timeout, return failure

        *Data++ = I2C0DAT;

        if (len > 1)
            I2C0CONSET = 0x04 | 0x40;  // Set ACK and STO bits for continuing transmission
        else
            I2C0CONSET = (0x10 | 0x40);  // Set STO bit for stop condition

        len--;
    }

    I2C0CONSET = (0x10 | 0x40);  // Send Stop bit
    I2C0CONCLR = 0x2C;

    return 1;  // Read operation successful
}

void WriteEeprom0(char SlaveAddress, unsigned char *Data, char len)
{
    WriteDataI2c0(SlaveAddress, Data, len);
}

void ReadEeprom0(char SlaveAddress, unsigned char *Data, char len)
{
    WriteDataI2c0(SlaveAddress, &addr, 0); // Write address to the EEPROM
    ReadDataI2c0((SlaveAddress + 0x01), Data, len); // Read data from the EEPROM
}
```

# RTOS

![_](https://github.com/mathur-exe/ESD/blob/main/img/ID_RTOS.png)

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

## RTOS Semaphore

![_](https://github.com/mathur-exe/ESD/blob/main/img/ID_RTOS_Sem.png)

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

## RTC

![_](https://github.com/mathur-exe/ESD/blob/main/img/RTC%20Interfacing%20Diagram.png)

```
#include "LPC214x.h"

#define LCD (0xFF << 8) // Value to control LCD
#define RS (1 << 5)     // Register Select pin
#define RW (1 << 6)     // Read/Write pin
#define EN (1 << 7)     // Enable pin

void delay_ms(unsigned int x, int y);
void LCD_data(unsigned int x);
void LCD_cmd(unsigned int x);
void LCD_ini();
void LCD_str(char x[]);
void RTC_init();
int hextodec(int x);

int main()
{
    PINSEL0 = 0x00000000; // Configure Pin Select Block 0 as GPIO
    IODIR0 = 0xFFFFFFFF;  // Set all pins of GPIO Port 0 as output

    int hour, min, sec, day, month, year;

    LCD_ini();  // Initialize the LCD
    RTC_init(); // Initialize the RTC

    LCD_cmd(0x80); // Set the cursor to the beginning of the first line
    LCD_str("Time:"); // Display "Time:" on the LCD
    LCD_cmd(0xC0); // Set the cursor to the beginning of the second line
    LCD_str("Date:"); // Display "Date:" on the LCD

    while (1)
    {
        hour = hextodec(HOUR);  // Get the current hour from the RTC
        min = hextodec(MIN);    // Get the current minute from the RTC
        sec = hextodec(SEC);    // Get the current second from the RTC
        day = hextodec(DOM);    // Get the current day from the RTC
        month = hextodec(MONTH); // Get the current month from the RTC
        year = hextodec(YEAR);   // Get the current year from the RTC

        delay_ms(100, 10); // Delay for a short period of time

        LCD_cmd(0x85); // Set the cursor to the position to display the hour
        LCD_data((hour / 10) + '0'); // Display the tens digit of the hour
        LCD_data((hour % 10) + '0'); // Display the ones digit of the hour

        LCD_cmd(0x87); // Set the cursor to the position to display the ":" separator
        LCD_data(':'); // Display the ":" separator

        LCD_cmd(0x88); // Set the cursor to the position to display the minute
        LCD_data((min / 10) + '0'); // Display the tens digit of the minute
        LCD_data((min % 10) + '0'); // Display the ones digit of the minute

        LCD_cmd(0x8A); // Set the cursor to the position to display the ":" separator
        LCD_data(':'); // Display the ":" separator

        LCD_cmd(0x8B); // Set the cursor to the position to display the second
        LCD_data((sec / 10) + '0'); // Display the tens digit of the second
        LCD_data((sec % 10) + '0'); // Display the ones digit of the second

        LCD_cmd(0xC5); // Set the cursor to the position to display the day
        LCD_data((day / 10) + '0'); // Display the tens digit of the day
        LCD_data((day % 10) + '0'); // Display the ones digit of the day

        LCD_cmd(0xC7); // Set the cursor to the position to display the "/" separator
        LCD_data('/'); // Display the "/" separator
        LCD_cmd(0xC8); // Set the cursor to the position to display the month
        LCD_data((month / 10) + '0'); // Display the tens digit of the month
        LCD_data((month % 10) + '0'); // Display the ones digit of the month

        LCD_cmd(0xCA); // Set the cursor to the position to display the "/" separator
        LCD_data('/'); // Display the "/" separator

        LCD_cmd(0xCB); // Set the cursor to the position to display the year
        LCD_data((year / 10) + '0'); // Display the tens digit of the year
        LCD_data((year % 10) + '0'); // Display the ones digit of the year
}
void RTC_init()
{
    CCR = 0x11;    // Configure the RTC control register (CCR)
    HOUR = 12;     // Set the initial hour value in the RTC
    MIN = 8;       // Set the initial minute value in the RTC
    SEC = 10;      // Set the initial second value in the RTC
    DOM = 23;      // Set the initial day of the month value in the RTC
    MONTH = 3;     // Set the initial month value in the RTC
    YEAR = 23;     // Set the initial year value in the RTC
}

int hextodec(int value)
{
    int temp;
    temp = value & 0x0F;        // Extract the lower 4 bits of the value
    value = (value & 0xF0) >> 4; // Extract the upper 4 bits of the value
    value = value * 16;          // Multiply the upper bits by 16 to shift them to the left

    if (temp <= 9)
    {
        value = value + temp;  // If the lower bits are less than or equal to 9, add them to the result
    }
    else if (temp > 9)
    {
        switch (temp)
        {
            case 0x0A:
                value = value + 10;  // If the lower bits are 0x0A, add 10 to the result
                break;
            case 0x0B:
                value = value + 11;  // If the lower bits are 0x0B, add 11 to the result
                break;
            case 0x0C:
                value = value + 12;  // If the lower bits are 0x0C, add 12 to the result
                break;
            case 0x0D:
                value = value + 13;  // If the lower bits are 0x0D, add 13 to the result
                break;
            case 0x0E:
                value = value + 14;  // If the lower bits are 0x0E, add 14 to the result
                break;
            case 0x0F:
                value = value + 15;  // If the lower bits are 0x0F, add 15 to the result
                break;
        }
    }
    return value;  // Return the decimal result
}

void LCD_data(unsigned int x)
{
    IOCLR0 = 0x0000FF00;  // Clear the data lines of the LCD (P0.8 to P0.15)
    x = (x << 8);         // Shift the data to the correct position (P0.8 to P0.15)
    IOSET0 = x;           // Set the data lines of the LCD according to the value
    IOSET0 = RS;          // Set the Register Select (RS) pin to indicate data
    IOCLR0 = RW;          // Clear the Read/Write (RW) pin to write data
    IOSET0 = EN;          // Set the Enable (EN) pin to enable data transfer
    delay_ms(100, 10);    // Delay for a short period of time
    IOCLR0 = EN;          // Clear the Enable (EN) pin to disable data transfer
}
void LCD_cmd(unsigned int x)
{
    IOCLR0 = 0x0000FF00;    // Clear the data lines of the LCD (P0.8 to P0.15)
    x = (x << 8);           // Shift the command to the correct position (P0.8 to P0.15)
    IOSET0 = x;             // Set the data lines of the LCD according to the command
    IOCLR0 = RS;            // Clear the Register Select (RS) pin to indicate a command
    IOCLR0 = RW;            // Clear the Read/Write (RW) pin to write data
    IOSET0 = EN;            // Set the Enable (EN) pin to enable command execution
    delay_ms(100, 10);      // Delay for a short period of time
    IOCLR0 = EN;            // Clear the Enable (EN) pin to disable command execution
}

void LCD_ini()
{
    LCD_cmd(0x38);          // Initialize the LCD: 8-bit mode, 2 lines, 5x7 dot matrix
    LCD_cmd(0x0E);          // Display ON, cursor ON, cursor blinking OFF
    LCD_cmd(0x01);          // Clear the LCD display
    LCD_cmd(0x06);          // Set the cursor to increment mode, no display shifting
}

void LCD_str(char x[])
{
    int i;
    for (i = 0; x[i] != '\0'; i++)
    {
        LCD_data(x[i]);     // Send each character of the string to the LCD
    }
}

void delay_ms(unsigned int x, int y)
{
    unsigned int i, j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            // Delay loop for a specified number of milliseconds
        }
    }
}

```