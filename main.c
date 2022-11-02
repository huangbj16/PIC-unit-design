/*
 * File:   main.c
 * Author: bingjian
 *
 * Created on November 1, 2022, 4:35 PM
 */


#include <xc.h>
#include "config.h"
#include <pic16f15214.h>

#define _XTAL_FREQ 1000000

uint8_t temp = 0;
unsigned char data = 0;
uint8_t counter = 0;

void main(void) {
    OSCFRQ = 0x02;
    HFOEN = 1;
    TRISA &= (0b11111011);
    RA2 = 0;
//    RA5 = 0;
//    RA4 = 0;
//    while(1){
//        for(int i=0; i<160; i++){
//            RA5 = 1; RA4 = 0;
//            __delay_ms(3);
//            RA5 = 0; RA4 = 1;
//            __delay_ms(3);
//        }
//        RA5 = 1; RA4 = 1;
//        RA5 = 0; RA4 = 0;
//        __delay_ms(1000);
//    }
    
    //setup timer0 = 1MHz for interrupt
    T0CON0 = (0b10000000);
    T0CON1 = (0b01000000);
    TMR0H = (0xFF);
    TMR0L = (0x00);
    //setup interrupt
    GIE = 1;
    PEIE = 1;
//    TMR0IE = 1;
//    TMR0IF = 0;
    
    //setup EUSART, async
    BRGH = 1;
    BRG16 = 0;
    SP1BRGH = 0;
    SP1BRGL = 25;//10417 baud rate with 0% error
    RA5PPS = 0x05;//set RA5 as TX output
    RX1PPS = 0x04;//set RA4 as RC input
    ANSA4 = 0;
    SYNC = 0;
    SPEN = 1;
    //set interrupt for write
    TXEN = 1;
    CREN = 1;
    RC1IE = 1;
//    TX1IE = 1;
    
    while(1){
//        if(OERR == 1){
//            CREN = 0;
//            __delay_ms(5);
//            CREN = 1;
//        }
    }
    
    return;
}

void __interrupt() ISR(void) {
    counter++;
    if(RC1IF == 1){
        data = RC1REG;
        RC1IF = 0;
        TX1IE = 1;//enable writing
    }
    if(TX1IF == 1){// write new data to TX1REG
        if(data != 0){
            TX1REG = data;
            data = 0;
            TX1IE = 0;//disable writing
//            TX1IF = 0;
        }
    }
//    if(TMR0IF == 1){
//        temp = 1-temp;
//        RA5 = temp;
//        TMR0IF = 0;
//    }
}
