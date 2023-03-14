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
        LDR R4, = 0x05 LDR R5, [R3], #04
        SUB R4, R4, #01
        MOV R7, #00
L1      LDR R5, [R3], #04
        CMP R1, R5
        BHS L2
        MOVS R1, R5
L2      SUBS R4, R4, #01
        BNE L1
STOP    B STOP
    AREA PROGI, DATA, READWRITE
datal DCD 0x05, 0x03, 0x08, 0x04, 0x01
    END