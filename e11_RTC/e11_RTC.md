- only Flash Magic

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