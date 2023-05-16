
![]()

- [https://rl.se/gprmc](https://rl.se/gprmc)
- Upload hex file to controller using Flash Magic
- Open HyperTerminal

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