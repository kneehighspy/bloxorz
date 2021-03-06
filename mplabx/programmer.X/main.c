#define _XTAL_FREQ 16000000

// PIC18F45K50 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLSEL = PLL4X   // PLL Selection (4x clock multiplier)
#pragma config CFGPLLEN = OFF   // PLL Enable Configuration bit (PLL Disabled (firmware controlled))
#pragma config CPUDIV = NOCLKDIV// CPU System Clock Postscaler (CPU uses system clock (no divide))
#pragma config LS48MHZ = SYS24X4// Low Speed USB mode with 48 MHz system clock (System clock at 24 MHz, USB clock divider is set to 4)

// CONFIG1H
#pragma config FOSC = INTOSCIO  // Oscillator Selection (Internal oscillator)
#pragma config PCLKEN = ON      // Primary Oscillator Shutdown (Primary oscillator enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config nPWRTEN = OFF    // Power-up Timer Enable (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable (BOR enabled in hardware (SBOREN is ignored))
#pragma config BORV = 190       // Brown-out Reset Voltage (BOR set to 1.9V nominal)
#pragma config nLPBOR = OFF     // Low-Power Brown-out Reset (Low-Power Brown-out Reset disabled)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored))
#pragma config WDTPS = 32768    // Watchdog Timer Postscaler (1:32768)

// CONFIG3H
#pragma config CCP2MX = RC1     // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config T3CMX = RC0      // Timer3 Clock Input MUX bit (T3CKI function is on RC0)
#pragma config SDOMX = RB3      // SDO Output MUX bit (SDO function is on RB3)
#pragma config MCLRE = OFF      // Master Clear Reset Pin Enable (RE3 input pin enabled; external MCLR disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled)

// CONFIG5L
#pragma config CP0 = OFF        // Block 0 Code Protect (Block 0 is not code-protected)
#pragma config CP1 = OFF        // Block 1 Code Protect (Block 1 is not code-protected)
#pragma config CP2 = OFF        // Block 2 Code Protect (Block 2 is not code-protected)
#pragma config CP3 = OFF        // Block 3 Code Protect (Block 3 is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protect (Boot block is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protect (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Block 0 Write Protect (Block 0 (0800-1FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Block 1 Write Protect (Block 1 (2000-3FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Block 2 Write Protect (Block 2 (04000-5FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Block 3 Write Protect (Block 3 (06000-7FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Registers Write Protect (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protect (Boot block (0000-7FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protect (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Block 0 Table Read Protect (Block 0 is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Block 1 Table Read Protect (Block 1 is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Block 2 Table Read Protect (Block 2 is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Block 3 Table Read Protect (Block 3 is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protect (Boot block is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#include <stdint.h>

#include "receiver.h"

extern void sendBankCommand(uint8_t bank);


#define UART_BUFFER_SIZE 1024
volatile uint8_t uartBuf[UART_BUFFER_SIZE];
volatile uint16_t uartReadIndex = 0;
volatile uint16_t uartWriteIndex = 0;

void interrupt receiveData() {
    if (RCSTA1bits.OERR == 1) {
        RCSTA1bits.OERR = 0;
        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;
    }
    if (PIR1bits.RC1IF == 1) {
        uartBuf[uartWriteIndex] = RCREG1;
        uartWriteIndex++;
        if (uartWriteIndex == UART_BUFFER_SIZE) uartWriteIndex = 0;
    }
}

void initUart()
{
    TXSTA1bits.SYNC = 0;
    TXSTA1bits.TX9 = 0;
    TXSTA1bits.TXEN = 1;
    
    RCSTA1bits.RX9 = 0;
    RCSTA1bits.CREN = 1;
    RCSTA1bits.SPEN = 1;
    
    BAUDCON1bits.BRG16 = 1;
    TXSTA1bits.BRGH = 1;
    SPBRG1 = 15;

    // enable interrupts
    RCONbits.IPEN   = 1;
	INTCONbits.GIE  = 1;
	INTCONbits.PEIE = 1;

    // enable receiver interrupts, l�w priority
    PIE1bits.RCIE = 1;
}

void writePortA(uint8_t data)
{
    PORTA = data;
}

void writePortB(uint8_t data)
{
    PORTB = data;
}

void writePortC(uint8_t data)
{
    PORTC = data;
}

void writePortD(uint8_t data)
{
    PORTD = data;
}

void writePortE(uint8_t data)
{
    PORTE = data;
}

void writeTrisA(uint8_t data)
{
    TRISA = data;
}

void writeTrisB(uint8_t data)
{
    TRISB = data;
}

void writeTrisC(uint8_t data)
{
    // configure UART TX always as output, and UART RX always as input
    TRISC = (uint8_t) (data & 0b10111111 | 0x80);
}

void writeTrisD(uint8_t data)
{
    // RD7 for PB6 always input, changed to output just in sendBankCommand
    TRISD = (uint8_t) (data | 0x80);
}

void writeTrisE(uint8_t data)
{
    TRISE = data;
}

uint8_t readPortA()
{
    return PORTA;
}

uint8_t readPortB()
{
    return PORTB;
}

uint8_t readPortC()
{
    return PORTC;
}

uint8_t readPortD()
{
    return PORTD;
}

uint8_t readPortE()
{
    return PORTE;
}

uint8_t readChar()
{
    uint8_t data;
    while (uartReadIndex == uartWriteIndex) ;
    data = uartBuf[uartReadIndex];
    uartReadIndex++;
    if (uartReadIndex == UART_BUFFER_SIZE) uartReadIndex = 0;
    return data;
}

void writeChar(uint8_t data)
{
    while (TXSTA1bits.TRMT == 0) {}
    TXREG1 = data;
}

int main()
{
    // 16 MHz internal oscillator
    OSCCONbits.IRCF = 7;
    
    // all I/O pins are configured as digital
    ANSELA = 0;
    ANSELC = 0;

    // disable ADC module
    ADCON0 = 0;    

    // initial value of port C bits
    PORTC = 0xff;

    // configure all pins as input, but UART TX as output
    TRISA = 0xff;
    TRISB = 0xff;
    TRISC = 0b10111111;
    TRISD = 0xff;
    
    initUart();

    // communication loop
    while (1) {
        onData(readChar());
    } 

    return 0;
}
