/*
 * hexToBCD.c
 *
 *  Created on: Feb 27, 2025
 *      Author: chait
 */

#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

//---------------------------
//Hex to BCD Conversion
//Convert a Hex number to a BCD for display on LCD
//
//---------------------------
unsigned char adc_char[4] = {'0', '0', '0', '0'};
unsigned char timer_char[5] = {'0', '0', '0', '.', '0'};
unsigned int hex_val = 0x0000;
void ACDtoHEX(int ADC_value) {
    hex_val = ADC_value * 33;
    hex_val = hex_val * 297;
    hex_val = hex_val >> 12;
}

void HEXtoBCD(int hex_value){
    int value = 0;
    unsigned int i = 0;
    for (i = 0; i < 4; i++) {
        adc_char[i] = '0';
    }
    while (hex_value > 999){
        hex_value = hex_value - 1000;
        value = value + 1;
        adc_char[0] = 0x30 + value;
    }
    value = 0;
    while (hex_value > 99){
        hex_value = hex_value - 100;
        value = value + 1;
        adc_char[1] = 0x30 + value;
    }
    value = 0;
    while (hex_value > 9){
        hex_value = hex_value - 10;
        value = value + 1;
        adc_char[2] = 0x30 + value;
    }
    adc_char[3] = 0x30 + hex_value;
}
void TIMERtoBCD(int timer_value) {
    int value = 0;
    unsigned int i = 0;
    for (i = 0; i < 5; i++) {
            timer_char[i] = '0';
        }
    timer_char[3] = '.';
    while (timer_value > 999) {
        timer_value = timer_value - 1000;
        value = value + 1;
        timer_char[0] = 0x30 + value;
    }
    value = 0;
    while (timer_value > 99){
        timer_value = timer_value - 100;
        value = value + 1;
        timer_char[1] = 0x30 + value;
    }
    value = 0;
    while (timer_value > 9){
        timer_value = timer_value - 10;
        value = value + 1;
        timer_char[2] = 0x30 + value;
    }
    timer_char[4] = 0x30 + timer_value;
}
//-------------------------------------------
// ADC Line insert
// Take the HEX to BCD value in the array adc_char and place it
// in the desired location on the desired line of the display.
// char line => Specifies the line 1 thru 4
// char location => Is the location 0 thru 9
//
//-------------------------------------------------------------
void adc_line(char line, char location){
//-------------------------------------------------------------
int i;
unsigned int real_line;
real_line = line - 1;
for(i=0; i < 4; i++) {
display_line[real_line][i+location] = adc_char[i];
}
}
//-------------------------------------------------------------
void timer_line (char line, char location) {
    int i;
    unsigned int real_line;
    real_line = line - 1;
    for(i = 0; i < 5; i++) {
        display_line[real_line][i+location] = timer_char[i];
    }
}
