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