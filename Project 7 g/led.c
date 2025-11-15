/*
 * led.c
 * Description:Defines the initialization of LEDs
 *
 *  Created on: Jan 30, 2025
 *      Author: chait
 */
#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

void Init_LEDs(void){
//------------------------------------------------------------------------------
// LED Configurations
//------------------------------------------------------------------------------
// Turns off both LEDs
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
//------------------------------------------------------------------------------
}
