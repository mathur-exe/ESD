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