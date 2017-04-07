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

_FOSC(CSW_FSCM_OFF & FRC);
_FWDT(WDT_OFF);
_FBORPOR(MCLR_DIS);


/*int __attribute__((__weak__, __section__(".libc"))) 
read(int handle, void *buffer, unsigned int len)
{
 int i; 
 int readChar;
 switch (handle) {
 case 0: // stdout
 case 1: // stdin
 case 2: // stderr
 for (i = len; i; --i) {
 while (!U1STAbits.URXDA); // wait until UART1 rx char ready
 printf("after while loop \n");
 readChar = U1RXREG;
 *(char*)buffer++ = readChar;
 U1TXREG = readChar; // echo readChar back to serial out...
 
 // You may have to change your expected "enter key" values...
 // carriage return = 0x0D = '\r'
 // newline = 0x0A = '\n'
 if (readChar == 0x0D || readChar == 0x0A) {
 // These reset the terminal. I'm not sure if we need these.
// U1TXREG = '\r';
 U1TXREG = '\n';
 break; // break out of the for loop...
 }
 }
len -= i; // Calculate how many characters we read before breaking
 break;
 default:
 break;
 }
 
 return len;
}
*/


char RXbuffer[80];	//buffer used to store characters from serial port
int str_pos = 0; 	//position in the RXbuffer
/* pin for enablePWM and disablePWM
 * 
 * 0 - PWM1L
 * 1 - PWM1H
 * 2 - PWM2L
 * 3 - PWM2H
 * 4 - PWM3L
 * 5 - PWM3H
 */



void enablePWM(int pin){
    FLTACON = FLTACON|(0x100000000<<pin);
    
}

void disablePWM(int pin){
    FLTACON = FLTACON&(~(0x100000000<<pin));
}

void forward(int speed){
   
    for(int i = 0; i < 6; i++){
        enablePWM(i);
        //delay for like 1 sec? <--- **********
        disablePWM(i);
        //delay if necessary (dont think it'll be)
        
    }
}

void backward(int speed){
   
    for(int i = 5; i >= 0; i--){
        enablePWM(i);
        //delay for like 1 sec? <--- **********
        disablePWM(i);
        //delay if necessary (dont think it'll be)
        
    }
}



int main(void) {
  
    TRISB = 0;
    U1BRG = 11; // for baud rate
    U1MODEbits.UARTEN = 1;

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
    PDC2 = 0x0000;
    PDC3 = 0x0000;
    /**********DO SOMETHING ABOUT FLTACON for input L and H select ***************/
    
    //FLTACON = 0x030F; 
    
        PTCON = 0x8000;                 // Enabling the PWM module (OWM time base timer enable bit)

    /*end setting up pwm */
    __C30_UART = 1; 
    
    int i = 0;
    char c;
    
    while(1)
    {
    
        printf("start of loop\n");
        /*printf("loop\n");
        printf("start of loop\n");
        printf("start of loop\n");
        printf("start of loop\n");
      //printf("\rhello: %d\n", i);*/
       //j =  read(2, save_data, 256);
        //scanf("%s", save_data);
        //while (U1STAbits.URXDA == 0){printf("%d\n", i);__delay32(30000);};           
        //U1STAbits.OERR = 0;
        if (U1STAbits.URXDA == 1)
        {
            // If a '1' or '0' were received, set RD0 and RD1
            c = U1RXREG;
            if (c == '1') {printf("command 1: %c\n", c);}
            else if (c == '0') {printf("command 0: %c\n", c);}
            else {printf("string: %c\n", c);}
        }
        /*if(U1RXREG || U1STAbits.URXDA)
            printf("recieved: %s\n", U1RXREG);
        
        i++;*/
        
            


		//str_pos = 0;	//returns the pointer to position zero in the circular buffer
		//for(i=0;i<80;i++){RXbuffer[i] = '\0';}		//erases the buffer
        
        
        _LATB0 = 1 - _LATB0;
        __delay32(1200000);
        
    }
    
    return 0;
}