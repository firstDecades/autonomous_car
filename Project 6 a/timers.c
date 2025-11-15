/*
 * TimerB0.c
 * Description: will hold the interrupts and timer updates
 *  Created on: Feb 19, 2025
 *      Author: chait
 */

#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"




void Init_Timers (void) {
    Init_Timer_B0();
}
//--------------------------------------------------------------------
// Timer B0 initialization sets up both B0_0, B0_1-B0_2 and overflow
void Init_Timer_B0(void) {
    //TB0CTL = TBSSEL__SMCLK; // SMCLK source
    TB0CTL |= TBCLR; // Resets TB0R,
    TB0CTL |= TBSSEL__SMCLK; //clock to smclk
    TB0CTL |= ID__2; // Divide clock by 2 8,000,000/2 = 4,000,000/8 = 500,000
    TB0EX0 |= TBIDEX__7; // Divide clock by an additional 8
    TB0CTL |= MC__CONTINUOUS; // Continuous up to 0xFFFF and overflow


    //capture compare 0
    // #pragma vector = TIMER0_B0_VECTOR

    //Capture compare 0
    TB0CCR0 = TB0CCR0_INTERVAL; // CCR0
    TB0CCTL0 &= ~CCIFG; //Clear possible pending interrupt
    TB0CCTL0 |= CCIE; // CCR0 enable interrupt

    //capture compare 1,2, overflow
    //#pragma vector = TIMER0_B1_VECTOR

    //Capture compare 1
    TB0CCR1 = TB0CCR1_INTERVAL;// CCR1
    TB0CCTL1 &= ~CCIFG; //Clear possible pending interrupt
    //TB0CCTL1 |= CCIE; // CCR1 enable interrupt

    //Capture compare 2
    TB0CCR2 = TB0CCR2_INTERVAL;// CCR2
    TB0CCTL2 &= ~CCIFG; //Clear possible pending interrupt
    TB0CCTL2 |= CCIE; // CCR2 enable interrupt

    //overflow
    TB0CTL &= ~TBIE; // Disable Overflow Interrupt
    TB0CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}



