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

volatile unsigned int ms200Time_Sequence = 0;


void Init_Timers (void) {
    Init_Timer_B0();
    Init_Timer_B1();
    Init_Timer_B3();
}
//--------------------------------------------------------------------
// Timer B0 initialization sets up both B0_0, B0_1-B0_2 and overflow
void Init_Timer_B0(void) {
    //TB0CTL = TBSSEL__SMCLK; // SMCLK source
    TB0CTL |= TBCLR; // Resets TB0R,
    TB0CTL |= TBSSEL__SMCLK; //clock to smclk
    TB0CTL |= ID__8; // Divide clock by 8 8,000,000/8 = 1,000,000/8 = 125,000
    TB0EX0 |= TBIDEX__8; // Divide clock by an additional 8
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
    TB0CCTL1 |= CCIE; // CCR1 enable interrupt

    //Capture compare 2
    TB0CCR2 = TB0CCR2_INTERVAL;// CCR2
    TB0CCTL2 &= ~CCIFG; //Clear possible pending interrupt
    TB0CCTL2 |= CCIE; // CCR2 enable interrupt

    //overflow
    TB0CTL &= ~TBIE; // Disable Overflow Interrupt
    TB0CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}


//--------------------------------------------------------------------
// Timer B1 initialization sets up both B1_0, B1_1-B1_2 and overflow
void Init_Timer_B1(void) {
    //TB1CTL = TBSSEL__SMCLK; // SMCLK source
    TB1CTL |= TBCLR; // Resets TB1R,
    TB1CTL |= TBSSEL__SMCLK; //clock to smclk
    TB1CTL |= ID__8; // Divide clock by 8 8,000,000/8 = 1,000,000
    TB1EX0 |= TBIDEX__8; // Divide clock by an additional 8 1,000,000/8 = 125,000
    TB1CTL |= MC__CONTINUOUS; // Continuous up to 0xFFFF and overflow


    //capture compare 0
    // #pragma vector = TIMER0_B0_VECTOR

    //Capture compare 0
    TB1CCR0 = TB1CCR0_INTERVAL; // CCR0
    TB1CCTL0 &= ~CCIFG; //Clear possible pending interrupt
    TB1CCTL0 |= CCIE; // CCR0 enable interrupt

    //capture compare 1,2, overflow
    //#pragma vector = TIMER0_B1_VECTOR

    //Capture compare 1
    TB1CCR1 = TB1CCR1_INTERVAL;// CCR1
    TB1CCTL1 &= ~CCIFG; //Clear possible pending interrupt
    TB0CCTL1 |= CCIE; // CCR1 enable interrupt

    //Capture compare 2
    TB1CCR2 = TB1CCR2_INTERVAL;// CCR2
    TB1CCTL2 &= ~CCIFG; //Clear possible pending interrupt
    TB1CCTL2 |= CCIE; // CCR2 enable interrupt

    //overflow
    TB1CTL &= ~TBIE; // Disable Overflow Interrupt
    TB1CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}

void Init_Timer_B3(void) {
//------------------------------------------------------------------------------
// SMCLK source, up count mode, PWM Right Side
// TB3.1 P6.0 LCD_BACKLITE
// TB3.2 P6.1 R_FORWARD
// TB3.3 P6.2 R_REVERSE
// TB3.4 P6.3 L_FORWARD
// TB3.5 P6.4 L_REVERSE
//------------------------------------------------------------------------------
    TB3CTL = TBSSEL__SMCLK; // SMCLK
    TB3CTL |= MC__UP; // Up Mode
    TB3CTL |= TBCLR; // Clear TAR

    PWM_PERIOD = WHEEL_PERIOD; // PWM Period [Set this to 50005]

    TB3CCTL1 = OUTMOD_7; // CCR1 reset/set
    LCD_BACKLITE_DIMING = PERCENT_80; // P6.0 Right Forward PWM duty cycle

    TB3CCTL2 = OUTMOD_7; // CCR2 reset/set
    RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM duty cycle
    //RIGHT_FORWARD_SPEED = FAST;

    TB3CCTL3 = OUTMOD_7; // CCR3 reset/set
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.2 Right Reverse PWM duty cycle
    //RIGHT_REVERSE_SPEED = SLOW;

    TB3CCTL4 = OUTMOD_7; // CCR4 reset/set
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.3 Left Forward PWM duty cycle
   // LEFT_FORWARD_SPEED = FAST;

    TB3CCTL5 = OUTMOD_7; // CCR5 reset/set
    LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM duty cycle
    //LEFT_REVERSE_SPEED = SLOW;
    //------------------------------------------------------------------------------
}



