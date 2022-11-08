/*
 * File:   main.c
 * Author: bingjian
 *
 * Created on November 1, 2022, 4:35 PM
 */


#include <xc.h>
#include "config_18313.h"
#include <pic16f18313.h>

#define _XTAL_FREQ 4000000

uint8_t temp = 0;
unsigned char data = 0;
uint8_t counter = 0;
uint8_t enable = 0;
uint8_t flip = 0;
uint16_t countdown_timer = 0;

void main(void) {
    //set internal oscillator to 4MHz
    OSCCON1 = 0x60;
    OSCFRQ = 0x03;
    HFOEN = 1;
    TRISA &= (0b11111100);
    RA1 = 0;
    RA0 = 0;
    ANSA1 = 0;
    ANSA0 = 0;
    
    //setup timer0 = 1MHz for interrupt
    T0CON0 = (0b10000000);
    T0CON1 = (0b01000101);//sync clock, 32 prescale
    TMR0H = 91;
    TMR0L = 0;
    //setup interrupt
    GIE = 1;
    PEIE = 1;
    TMR0IE = 1;
    TMR0IF = 0;
    
    //setup EUSART, async
    BRGH = 1;
    BRG16 = 0;
    SP1BRGH = 0;
    SP1BRGL = 25;//9600 baud rate with 0.16% error
    RA5PPS = 0x14;//set RA5 as TX output
    RXPPS = 0x04;//set RA4 as RC input
    ANSA4 = 0;
    ANSA5 = 0;
    SYNC = 0;
    SPEN = 1;
    //set interrupt for write
    TXEN = 1;
    CREN = 1;
    RCIE = 1;
//    TXIE = 1;
    
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
    // serial processing
    if(RCIF == 1){
        data = RC1REG;
        RCIF = 0;
        if(data == 0xFF){
            if(enable == 1){
                //start trigger motor
                countdown_timer = 340;
                enable = 0;//reset state
            }
            TXIE = 1;
        }
        else{
            data--;
            if(data == 0){
                enable = 1;//command to enable current board
            }
            else{
                TXIE = 1;//enable writing, send to next
            }
        }
    }
    if(TXIF == 1 && TXIE == 1){// write new data to TX1REG
        if(data != 0){
            TX1REG = data;
            data = 0;
            TXIE = 0;//disable writing
        }
    }
    // square wave generator                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ator
    if(TMR0IF == 1 && countdown_timer != 0){
        flip = ~flip;
        RA1 = flip;
        RA0 = ~flip;
        countdown_timer--;
        TMR0IF = 0;
    }
}
