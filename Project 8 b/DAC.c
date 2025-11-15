/*
 * DAC.c
 *
 *  Created on: Mar 19, 2025
 *      Author: chait
 *      DAC control file
 */
#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"
volatile unsigned int DAC_data = DAC_Begin;

void Init_DAC(void) {
    SAC3DAC = DACSREF_0;
    SAC3DAC |= DACLSEL_0;

    SAC3OA = NMUXEN;        //SAC Negative input MUX control
    SAC3OA |= PMUXEN;       //SAC Positive input MUX control
    SAC3OA |= PSEL_1;       //12-bit reference DAC source selected
    SAC3OA |= NSEL_1;       //select negative pin input
    SAC3OA |= OAPM;         //select low speed and low power mode
    SAC3PGA = MSEL_1;       //set OA as buffer mode
    SAC3OA |= SACEN;        //enable SAC
    SAC3OA |= OAEN;         //enable OA

    DAC_data = DAC_Begin;   //starting low value for DAC output
    SAC3DAT = DAC_data;     //initial DAC data

    TB0CTL |= TBIE;          //Timer B0 overflow interrupt enable
    P1OUT |= RED_LED;

    SAC3DAC |= DACEN;       //enable DAC
}

