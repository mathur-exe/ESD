// Addition of Five 32 bit numbers
AREA PROGRAM ,CODE,READONLY
    ENTRY
    LDR R3, = DATA1
    LDR R4, = COUNT
    LDR R4, [R4]
    MOV R6, #0x00
    MOV R7, #00
L1      LDR R5, [R3], #4
    ADDS R6,R6,R5
    BCC L2                  // BCC: clear carry
    ADD R7,R7, #1           
    // if the result is greater than 0xFFFFFFFF, the program increments register R7 by 1 using the "ADD" instruction
L2      SUBS R4,R4, #1      // loop continues until reg R4 = 0
    BNE L1
STOP    B STOP
    AREA PROGRAM, CODE, READWRITE
DATA1 DCD 0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF
COUNT DCD 0X05
    END

AREA PROGI, CODE, READONLY
        ENTRY
        LDR R5, DATA1
        MOV RO, #05
        MOV R4, #00
L1        LDR R6, [R5], #4
        ADD R4, R4, R6
        SUBS RO, RO, #1
        BNE L1
STOP    B STOP
    AREA PROGI, DATA, READWRITE
DATA1 DCD 0xFFFF0000, 0x00001111, OxAAAA1111, OxAAA92222, 0xAAAA2221, 0xFFFFFFE
    END

// Largest Number
    AREA PROG1, CODE, READONLY
        ENTRY
        LDR R3, datal
        LDR R4, = 0x05 
        LDR R5, [R3], #04   //#04 represents that R3 should be incremented after 4 reads
        SUB R4, R4, #01
        MOV R7, #00
L1      LDR R5, [R3], #04
        CMP R1, R5          
        // compares values in R1 to values in R5. R5>R1: jump to L2 else R1 stored in R5     
        // CMP: performs sub but doesn't sure result
        BHS L2
        MOVS R1, R5
L2      SUBS R4, R4, #01    //SUBS: substracts 2 operants and put res in dest_reg
        BNE L1              // BNE: boolean not equal 
STOP    B STOP
    AREA PROGI, DATA, READWRITE
datal DCD 0x05, 0x03, 0x08, 0x04, 0x01
    END


// Smallest Number 
    AREA PROGI, CODE, READONLY
        ENTRY
        LDR R3, datal
        LDR R4, 0x05
L1      LDR R5, [R3], #04
        SUB R4, R4, #01
        MOV R7, #00
        LDR R5, [R3], #04
        CMP R1, R5
        BLO L2              //checks C_flag and branches to L2
        MOVS R1, RS
L2      SUBS R4, R4, #01
        BNE L1
STOP    B STOP
    AREA PROGI, DATA, READWRITE 
datal DCD 0x05, 0x03, 0x08, 0x04, 0x01 
        END

// Count of Odd Numbers
    AREA PROGI, DATA, READONLY
        ENTRY
        LDR R5, datal
        LDR RO, 0x05
        MOV R7, #00
L1      LDR R4, [R5], #04
        MOVS R1, R4, LSR #01
        BCC 12      //if C_flag is clear, branch to 12 
        // BCC: Branch if carry clear
        ADD R7, R7, #01
L2      SUB RO, RO, #01
        BNE L1
STOP    B STOP
        AREA PROGI, DATA, READWRITE
datal   DCD 0xABCDEF12, 0xDCABFE43, 0xEEEEFFFF, 0xAAAABBBB, 0xCCCCCCC1 
        END 

// LED on/off
/*
    Instructons for LED on / off
    1. In GPIO 0 dialogue box (OFF)
    - IO0DIR 0x000000FF
    - IO0SET 0x00000000
    - IO0PIN 0x82FFFF00
    - Pins 0xF2FFFF00
    2. In GPIO 0 dialogue box (ON)
    - IO0DIR 0x000000FF
    - IO0SET 0x000000FF
    - IO0PIN 0x82FFFFFF
    - Pins 0xF2FFFFFF
*/
#include<LPC214x.h>
int Delay()
int c,d; for (c-1;c<-10; c++) for (d=1;d<-10; d++); return 0;
int main() {
    PINSELO= 0x00000000;
    IOODIR= 0x000000FF;
    while (1) (
        IO0SET 0x000000FF; //off
        Delay();
        IO0CLR= 0x000000FF; //on
        Delay();
    }
}

// PLL0 (Phase Lock Loop)
#include <LPC214X.h>
int main() {
    PLLOCFG=0X00000023;     //setting clk_freq to 35
    PLLOCON=0X00000001;     //enables PLL control reg
    PLLOFEED=0X000000AA;    // next 2 reg enable changes 
    PLLOFEED=0X00000055;
    while (! (PLLOSTAT & 0X00000400));  
    //waits the loop till PLL has locked onto the new freq
    PLLOCON=0X0000003;      //clear PLL enable bit, turn off PLL
    PLLOFEED=0X000000AA;    //next 2 lines, feed seq to disable PLL
    PLLOFEED=0X00000055;
    VPBDIV=0X00000002;      //divide peripheral_clk_freq by 2
    return 0;
}

// 16x2 LCD interfacing 
#include "lpc214x.h"
// define the bit position of LCD dat pins
#define LCD (0xff<<8)
#define RS (1<<5)   //RS (register select)
#define RW (1<<6)   //RW (Read/write)
#define EN (1<<7)   //EN (Enable)

void delay_fv(unsigned int x,int y);    //provides delay of x*y clk cycles
void lcd_display(unsigned int x);   
void cmd (unsigned int x);
void lcd_ini();
//void lcd_str(unsigned char *x);
int main() {
    int i;
    unsigned char a[] = {"MITWPU"};
    PINSEL0=0X00000000;
    IODIR0=0XFFFFFFFF;
    lcd_ini();
    cmd(0XC0);
    for(i=0;a[i]!='\0';i++)
        lcd_display(a[i]);
    return 0;
}
void delay_fv(unsigned int x,int y) {
    unsigned int i,j;
    for(i=0;i<x;i++)
    for(j=0;j<y;j++);
}
void lcd_display(unsigned int x) //at bit 0 to 7;x= 0x41 = 01000001
{
    /*
        displays a single char on LCD by setting the data pins to value of the char
        RS=1 - data being sent
        RW=0 - write operation 
        pulsing the EN pin to latch dat into the LCD, then providing a delay
    */
    IOCLR0 = 0x0000FF00; //bits 8 to 18 as 00000000
    x =(x<<8); //at bit 8 to 15;x= 0x41 = 01000001
    IOSET0 = x; // bits 8 to 18 as 01000001
    IOSET0 =RS; //RS=1 for data
    IOCLR0 =RW; //RW =0
    IOSET0 =EN; //high to low pulse on enable line
    delay_fv(100,10);
    IOCLR0 =EN;
}
void cmd (unsigned int x) {
    /*
        sends a cmd to LCD by setting data pins to cmd value
        RS 0: indicate a cmd
        RW 0: write operation
        EN: latch cmd into LCD
    */
    IOCLR0 = 0x0000FF00;
    x =(x<<8);
    IOSET0 = x;
    IOCLR0 =RS;
    IOCLR0 =RW;
    IOSET0 =EN;
    delay_fv(100,10);
    IOCLR0 =EN;
}
void lcd_ini() {
    cmd(0X38);          //sets lcd to 2 lines and 5x8 char_mode
    cmd(0X0e);          //turns on the display and sets the cursor to blink
    cmd(0X01);          //clear the display
    cmd(0X06);          //cursor auto-increment mode
}
/*void lcd_str(unsigned char x[])
{
int i;
for(i=0;x[i]!='\0';i++)
{
lcd_display(x[i]);
}
}
*/


// Graphical LCD display 
#include "lpc214x.h"
#include "image.h"
#define LCD_D0 1<<24
#define LCD_D1 1<<25
#define LCD_D2 1<<26
#define LCD_D3 1<<27
#define LCD_D4 1<<28
#define LCD_D5 1<<29
#define LCD_D6 1<<30
#define LCD_D7 1<<31
#define RS (1<<16)
#define RW (1<<17)
#define EN (1<<18)
#define CS1 (1<<19)
#define CS2 (1<<20)
#define TotalPage 8
#define FOSC 12000000
void delay(int k) ;
void GLCD_Command(int Command) 
{
    /* 
        GLCD command function 
        IOCLR1= (LCD_D0|LCD_D1|LCD_D2|LCD_D3|LCD_D4|LCD_D5|LCD_D6|LCD_D7);
        These bits are set to '1', rest are zeros
    */
    IOCLR1=0xFF000000;
    Command = Command<<24;
    IOSET1 = Command;
    /* Copy command on data pin */
    IOCLR1 = RS; /* Make RS LOW to select
    command register */
    IOCLR1 = RW;
    /* Make RW LOW to select write
    operation */
    IOSET1 = EN;/* Make HIGH to LOW transition on Enable pin */
    delay(5);
    IOCLR1 = EN;
}
void GLCD_Data(int Data) {
    /* GLCD data function */
    //IOCLR1= (LCD_D0|LCD_D1|LCD_D2|LCD_D3|LCD_D4|LCD_D5|LCD_D6|LCD_D7);
    IOCLR1=0xFF000000;Data = Data << 24;
    IOSET1 = Data; /* Copy data on data pin */
    IOSET1 = RS; /* Make RS HIGH to select data register */
    IOCLR1 = RW; /* Make RW LOW to select write operation */
    IOSET1 = EN;/* Make HIGH to LOW transition on Enable pin */
    delay(5);
    IOCLR1 = EN;
}
void GLCD_Init() {
    /* 
        GLCD initialize function
        IODIR1|= (LCD_D0|LCD_D1|LCD_D2|LCD_D3|LCD_D4|LCD_D5|LCD_D6|LCD_D7|RS|RW|EN|CS1|CS2); 
    */

    PINSEL1=0x00000000;
    Configure all pins as output
    IODIR1|= 0xFFFF0000;
    IOSET1 = (CS1 | CS2); // Select both left & right half of display
    delay(20);
    GLCD_Command(0x3E);     //Display OFF
    GLCD_Command(0x40);     //Set Y address (column=0)
    GLCD_Command(0xB8);     //Set x address (page=0)
    GLCD_Command(0xC0);     //Set z address (start line=0)
    GLCD_Command(0x3F);     /* Display ON */
}
void GLCD_ClearAll()
{
    int column,page;
    /* GLCD all display clear function */
    for(page=0;page<8;page++) {
        // Print 16 pages i.e. 8 page of each controller 
        IOSET1 = CS1;
        IOCLR1 = CS2;
        GLCD_Command(0x40);
        GLCD_Command((0xB8+page));
        /* 
            If yes then change segment
            Set Y address (column=0) 
            Increment page address 
        */
        for(column=0;column<128;column++) {}
        if (column == 64) {
            IOCLR1 = CS1;                   //If yes then change segment
            IOSET1 = CS2;
            GLCD_Command(0x40);             //Set Y address (column=0)
            GLCD_Command((0xB8+page));      //Increment page address 
        }
    GLCD_Data(0);       //Print 64 column of each page
    }
}
void GLCD_String(const char *image) {
    // GLCD string write function 
    int column,page;
    for(page=0;page<8;page++) /* Print 16 pages i.e. 8 page of each half ofdisplay */
    {
        IOSET1 = CS1;
        IOCLR1 = CS2;
        GLCD_Command(0x40);
        GLCD_Command((0xB8+page));
        /* If yes then change segment
        controller */
        /* Set Y address (column=0) */
        /* Increment page address */
        for(column=0;column<128;column++)
        {
            if (column == 64) {
                IOCLR1 = CS1; //If yes then change segment
                IOSET1 = CS2;
                GLCD_Command(0x40); //Set Y address (column=0)
                GLCD_Command((0xB8+page));  //Increment page address
            }
            GLCD_Data(*image++); //Print 64 column of each page
        }
    }
}
// LCD Delay Function
void delay(int k) {
    int a,b;
    for(a=0;a<=k;a++)
    for(b=0;b<100;b++);
}
int main(void) {
    VPBDIV = 0X01;
    delay(1000);
    GLCD_Init(); /* Initialize GLCD */
    GLCD_ClearAll(); /* Clear all GLCD display */
    GLCD_String(img); /* Display image on GLCD display */
    while(1);
}


// RTC
#include"LPC214x.h"
#define LCD (0xff<<8)
#define RS (1<<5)
#define RW (1<<6)
#define EN (1<<7)void delay_ms(unsigned int x,int y);
void LCD_data(unsigned int x);
void LCD_cmd (unsigned int x);
void LCD_ini();
void LCD_str(char x[]);
void RTC_init();
int hextodec(int x);
int main() {
    PINSEL0=0X00000000;
    IODIR0=0XFFFFFFFF;
    int hour, min, sec, day, month, year;
    LCD_ini();
    RTC_init();
    LCD_cmd(0x80);
    LCD_str("Time:");
    LCD_cmd(0xC0);
    LCD_str("Date:");
    while(1){
        hour= hextodec(HOUR);
        min= hextodec(MIN);
        sec= hextodec(SEC);
        day= hextodec(DOM);month= hextodec(MONTH);
        year= hextodec(YEAR);
        delay_ms(100,10);
        LCD_cmd(0x85);
        LCD_data((hour/10)+'0');
        LCD_data((hour%10)+'0');
        LCD_cmd(0x87);
        LCD_data(':');
        LCD_cmd(0x88);
        LCD_data((min/10)+'0');
        LCD_data((min%10)+'0');
        LCD_cmd(0x8A);
        LCD_data(':');
        LCD_cmd(0x8B);
        LCD_data((sec/10)+'0');
        LCD_data((sec%10)+'0');
        LCD_cmd(0xC5);
        LCD_data((day/10)+'0');
        LCD_data((day%10)+'0');
        LCD_cmd(0xC7);
        LCD_data('/');
        LCD_cmd(0xC8);
        LCD_data((month/10)+'0');
        LCD_data((month%10)+'0');
        LCD_cmd(0xCA);LCD_data('/');
        LCD_cmd(0xCB);
        LCD_data((year/10)+'0');
        LCD_data((year%10)+'0');
    }
}
void RTC_init() {
    CCR=0x11;
    HOUR= 14;
    MIN= 30;
    SEC= 10;
    DOM= 23;
    MONTH= 02;
    YEAR= 23;
}
int hextodec (int value) {
    int temp;
    temp= value & 0x0f;
    value= (value & 0xf0)>>4;
    value= value*16;
    if (temp <= 9)
        value = value + temp;
    else if (temp > 9) {
        switch (temp) {
        case 0x0a:
            value= value + 10;
            break;
        case 0x0b:
            value= value + 11;
            break;
        case 0x0c:
            value= value + 12;
            break;
        case 0x0d:
            value= value + 13;
            break;
        case 0x0e:
            value= value + 14;
            break;
        case 0x0f:
            value= value + 15;
            break;
        }
    }
    return(value);
}
void LCD_data(unsigned int x) {
    IOCLR0 = 0x0000FF00;
    x =(x<<8);
    IOSET0 = x;
    IOSET0 =RS;
    IOCLR0 =RW;
    IOSET0 =EN;
    delay_ms(100,10);
    IOCLR0 =EN;
}
void LCD_cmd (unsigned int x) {
    IOCLR0 = 0x0000FF00;
    x =(x<<8);
    IOSET0 = x;IOCLR0 =RS;
    IOCLR0 =RW;
    IOSET0 =EN;
    delay_ms(100,10);
    IOCLR0 =EN;
}
void LCD_ini() {
    LCD_cmd(0X38);
    LCD_cmd(0X0e);
    LCD_cmd(0X01);
    LCD_cmd(0X06);
}
void LCD_str(char x[]) {
    int i;
    for(i=0;x[i]!='\0';i++)
        LCD_data(x[i]);
}
void delay_ms(unsigned int x,int y) {
    unsigned int i,j;
    for(i=0;i<x;i++)
    for(j=0;j<y;j++);
}