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

![PINSEL0]()