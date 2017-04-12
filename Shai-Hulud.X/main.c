/*
 * File:   main.c
 * Author: Code
 *
 * Created on April 1, 2017, 3:25 PM
 */

#include "xc.h"
#include <stdio.h>
#include <libpic30.h>
#include <p30F4011.h>
#include <time.h>

#define Fclock 7372800
#define Fcy Fclock/4

_FOSC(CSW_FSCM_OFF & FRC);
_FWDT(WDT_OFF);
_FBORPOR(MCLR_DIS);


char RXbuffer[80];	//buffer used to store characters from serial port
int str_pos = 0; 	//position in the RXbuffer
char current_command;
int changeFlag;
/* pin for enablePWM and disablePWM
 * 
 * 0 - PWM1L
 * 1 - PWM1H
 * 2 - PWM2L
 * 3 - PWM2H
 * 4 - PWM3L
 * 5 - PWM3H
 */

// delay and check whether the current command is updated. If it is, return -1. if not return 0;
int delayMS(int milliseconds){
    __delay32((Fcy/1000)*milliseconds);
    
    if(U2STAbits.URXDA == 0) // if buffer is empty, do nothing
        return 0;
    
    char c = U2RXREG; // if the buffer is non-empty, and the current command is the same, do nothing
    if(current_command == c){
        return 0;
    }
    current_command = c; // update current command are return -1;
    return -1;  
}

void enablePWM(int pin){
    FLTACON = FLTACON|(0x100000000<<pin);
    
}

void disablePWM(int pin){
    FLTACON = FLTACON&(~(0x100000000<<pin));
}

void forward(int speed){
    int i;
    for(i = 0; i < 6; i++){
        enablePWM(i);
        if(delayMS(400) == -1){
            FLTACON = 0;
            return;
        }
        disablePWM(i);
        //delay if necessary (dont think it'll be) 
    }
}

void backward(int speed){
    int i;
    for(i = 5; i >= 0; i--){
        enablePWM(i);
        if(delayMS(400) == -1){
            FLTACON = 0;
            return;
        }
        disablePWM(i);
        //delay if necessary (dont think it'll be)
    }
}

 void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void ) 
 { 
    char c = U2RXREG;

    //if(c == 'i'){
     //   printf("interrupt happen\n");            
   // }
    
    current_command =  c;
    IFS1bits.U2RXIF = 0; // Clear RX Interrupt flag 
 } 
 
 unsigned int readADC(int channel)
{
    ADCHS = channel;          // Select the requested channel
    ADCON1bits.SAMP = 1;      // start sampling
    __delay32(18);            // 10us delay @ 1.8 MIPS
    ADCON1bits.SAMP = 0;      // start Converting
    while (!ADCON1bits.DONE); // Shouldn't take 12 * Tad = 1.2us
    return ADCBUF0;
}

int main(void) {
    //TRISB = 0 // 1 for sensors
    _TRISB0 = 0;
    _TRISB1 = 1;
    
    ADPCFG = 0xFF01;     // Lowest 8 PORTB pins (except 0th) are analog inputs
    ADCON1 = 0;          // Manually clear SAMP to end sampling, start conversion
    ADCON2 = 0;          // Voltage reference from AVDD and AVSS
    ADCON3 = 0x0005;     // Manual Sample, ADCS=5 -&gt; Tad = 3*Tcy = 0.1us
    ADCON1bits.ADON = 1; // Turn ADC ON
    
    U2BRG = 11; // for baud rate
    
     /* setting up UART interrupts*/
    U2STAbits.URXISEL = 0; // interrupt after 1 character is received
    
    IPC6bits.U2RXIP = 3; // set priority of interrupt (7 - highest, 0 - lowest)
    IFS1bits.U2RXIF = 0; // reset the interrupt flag to 0 (interrupt has not occurred)
    IEC1bits.U2RXIE = 1; // Enable UART RX interrupt     
    
    U2MODEbits.UARTEN = 1;

   
    
    
    
    
    
    /*setting up pwm*/ 
    /* FLTACON, FLTBCON : fault a control register: decides PWM H or L */
    
    //TRISbits.TRISB2 = 1;
    //TRISbits.TRISB3 = 1;
    // PTPER = (Fcy/PWMfrequency) - 1 
    PTPER = 0x0032;                  // 0x0032 = 50
                         
                                           // Frequency - approx.200KHz
    
      /**********DO SOMETHING ABOUT FLTACON for input L and H select ***************/
    FLTACON = 0x0300; 
    /**** cant select and do pwm at the same time ****/
    
    //    SEVTCMP = 0xFFFF;
    PWMCON1 = 0x0FFF;            // RE0,RE2 are configured as PWM outputs whereas rest are Normal I/Os(PWM1L & PWM2L)
    PWMCON2 = 0x0000;            // Configured to obtain Duty Cycle through PDC registers
    PTMR  = 0x0000;
 
    PDC1 = 0x0032; // 0x32 = 50 , 50% duty cycle 
    PDC2 = 0x0032;
    PDC3 = 0x0032;
    /**********DO SOMETHING ABOUT FLTACON for input L and H select ***************/
    
    //FLTACON = 0x030F; 
    
    PTCON = 0x8000;                 // Enabling the PWM module (OWM time base timer enable bit)

    /*end setting up pwm */
    __C30_UART = 2; 
    
    int i = 0;
    char c;
    while(1)
    {
    
 
        if (U2STAbits.URXDA == 1)
        {
            // If a '1' or '0' were received, set RD0 and RD1
            //if (c == '1') {printf("command 1: %c\n", c);}
            //else if (c == '0') {printf("command 0: %c\n", c);}
            //else {printf("string: %c\n", c);}
            /*
            if(changeFlag == 0){
            c = U2RXREG;
            if((c == 'w')||(c == 'a')||(c == 's')||(c == 'd')||(c == 'n')){
                current_command = c;
            }
            }else{
                
                changeFlag = 0;
            }
                 
        */
            switch(current_command){
                case 'w': //forward
                    forward(0);
                    break;
                    
                case 's' ://backward
                    backward(0);
                    break;
               
                case 'a': // turn left
                        
                    break;
                case 'd': // turn right
                    
                    break;
                case 'n': //neutral 
                    FLTACON = 0;
                    break;
                default: // same as neutral
                    FLTACON = 0;
                    break;
            }
        
        
        
        
        
        }
        /*if(U1RXREG || U1STAbits.URXDA)
            printf("recieved: %s\n", U1RXREG);
        
        i++;*/
        
            


		//str_pos = 0;	//returns the pointer to position zero in the circular buffer
		//for(i=0;i<80;i++){RXbuffer[i] = '\0';}		//erases the buffer
        
        
        _LATB0 = 1 - _LATB0;
        __delay32(120000);
        
        //simple ADC readout
        int n = readADC(1);
        printf("adc -> %d\n", n);
        8
    }
    
    return 0;
}