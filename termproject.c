// Isaac Krapp
// ECE376 Term Project 

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

// Regular Setup
#pragma config OSC = INTIO67    
#pragma config WDT = OFF        
#pragma config LVP = OFF        
#pragma config MCLRE = OFF      

#define _XTAL_FREQ 16000000     // Match oscill frequency
#define HEATER PORTCbits.RC1    // Heater control (active high)
#define FAN PORTCbits.RC0       // Fan control (active high)
#define TARGET_TEMP 50.0        // Target heating temperature
#define SAMPLE_INTERVAL 1000    // 1 second sampling

// Funct prototypes
void setup_ADC(void);
float read_temp(void);
void UART_init(void);
void UART_write(char data);
void UART_send_string(const char *str);

void main(void) {
    OSCCON = 0x72;              // 16 MHz internal oscill
    TRISCbits.TRISC0 = 0;       // Fan = output
    TRISCbits.TRISC1 = 0;       // Heater = output
    setup_ADC();
    UART_init();

    while (1) {
        // Heating phase 
        HEATER = 1;
        UART_send_string("Heating started...\r\n");
        while (read_temp() < TARGET_TEMP); // Block until 50 deg C
        HEATER = 0;
        UART_send_string("Cooling phase started\r\n");

        // Cooling phase
        FAN = 1;                // Active cooling (use FAN = 0 for passive)
        for (int t = 0; t < 300; t++) { // 5 minutes (300 samples)
            float temp = read_temp();
            char buffer[32];
            sprintf(buffer, "%d,%.2f\r\n", t, temp);
            UART_send_string(buffer);
            __delay_ms(SAMPLE_INTERVAL);
        }
        FAN = 0;
        UART_send_string("Trial complete. Reset.\r\n");
        while(1);               // Halt until manual reset
    }
}

// Configure ADC for LM35 on AN0 
void setup_ADC(void) {
    TRISAbits.RA0 = 1;          // AN0 = input (LM35)
    ADCON1 = 0x0E;              // AN0 analog, Vref = AVDD/AVSS
    ADCON2 = 0x92;              // Right-justified, 4 TAD, Fosc/32
    ADCON0 = 0x01;              // Enable ADC, channel AN0
}

// Read temperature from LM35
float read_temp(void) {
    ADCON0bits.GO = 1;          // Start conversion
    while (ADCON0bits.GO);      // Wait (~20us)
    int adc_val = (ADRESH << 8) + ADRESL;
    return (adc_val * 500.0) / 1023.0; // LM35: 10mV/deg C
}

// Initialize UART 
void UART_init(void) {
    TRISCbits.TRISC6 = 0;       // TX = output (RC6)
    SPBRG = 25;                 // 9600 baud @ 16MHz
    RCSTA = 0x90;               // Enable UART
    TXSTA = 0x24;               // Async, 8-bit
}

void UART_write(char data) {
    while (!TXSTAbits.TRMT);    // Wait for TX ready
    TXREG = data;
}

void UART_send_string(const char *str) {
    while (*str) UART_write(*str++);
}
