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
char current_command = 'n';
int changeFlag;
char current_position = 'm';
char current_velocity = 'p';
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
    FLTACON = FLTACON|(0b100000000<<pin);
    
}

void disablePWM(int pin){
    FLTACON = FLTACON&(~(0b100000000<<pin));
}

void forward(int speed){
    int i;
    for(i = 0; i < 4; i++){
        enablePWM(i);
        /*if(delayMS(400) == -1){
            FLTACON = 0;
            return;
        }*/
        delayMS(400);
        disablePWM(i);
        //delay if necessary (dont think it'll be) 
    }
}

void backward(int speed){
    int i;
    for(i = 3; i >= 0; i--){
        enablePWM(i);
       /* if(delayMS(400) == -1){
            FLTACON = 0;
            return;
        }*/
        delayMS(400);
        disablePWM(i);
        //delay if necessary (dont think it'll be)
    }
}

void left(int speed){

    enablePWM(4);
}

void right(int speed){
    
    enablePWM(5);
}

void neutralPosition(int speed){
    disablePWM(4);
    disablePWM(5);
}

 void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void ) 
 { 
    char c = U2RXREG;

//    if(c == 'i'){
//        printf("interrupt happen\n");            
//    }
//    while(1){
//              _LATB0 = 1 - _LATB0;
//              delayMS(1000);
//    }
    

    
//    while(1){
//        backward(1);
//    }
    
    current_command =  c;
    IFS1bits.U2RXIF = 0; // Clear RX Interrupt flag 
    
    
    if (current_command == 'l' || current_command == 'r' 
                || current_command == 'm'){
            
            current_position = current_command;
    }
    
            
        switch(current_position){
            case 'l':
                left(0);
                break;
            case 'r':
                right(0);
                break;
            case 'm':
                neutralPosition(0);
                break;
            default:
                neutralPosition(0);
                break;
        }
        
        
    
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
    FLTACON = 0x030F; 
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
       // FLTACON = 0xFF0F;
        
//        if (U2STAbits.URXDA == 1)
//        {
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
                 
             * 
             *\
        */
        
     //   if(current_command == '')
        
//        
//        if(current_command == '')
//        
//            switch(current_command){
//                case 'f': // 
//                    forward(0);
//                    printf("hello\n");
//                    break;
//                case 'p' ://backward
//                    backward(0);
//                    break;
//               
//                case 'l': // turn left
//                    left(0);
//                    break;
//                case 'r': // turn right
//                    right(0);
//                    break;
//                case 'n': //neutral position
//                    neutralPosition(0);
//                    break;
//                default: // disable every acutator
//                    FLTACON = 0x000F;
//                    break;
//            }
//        
//        
        if(current_command == 'f' || current_command == 'b' 
                || current_command == 'p'){
            
            current_velocity = current_command;
        }
        
        
        switch(current_velocity){
            case 'f':
                forward(0);
                break;
            case 'b':
                backward(0);
                break;
            case 'p':
                disablePWM(0);
                disablePWM(1);
                disablePWM(2);
                disablePWM(3);
                break;
            default:
                disablePWM(0);
                disablePWM(1);
                disablePWM(2);
                disablePWM(3);
                break;
        }
        

        
        
        //}
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
    }
    
    return 0;
}