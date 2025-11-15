/*
 * display.c
 *  description: defines how display is updated
 *  Created on: Jan 30, 2025
 *      Author: chait
 */

#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

void Display_Process(void){
  if(update_display){
    update_display = 0;
    if(display_changed){
      display_changed = 0;
      Display_Update(0,0,0,0);
    }
  }
}

void LCD_update_ADC(void) {
    HEXtoBCD(ADC_V_Thumb >> 4);
    //strcpy(display_line[0], "V_Th      ");
    adc_line(1,5);
    HEXtoBCD(ADC_Left_Detect >> 3);
    //strcpy(display_line[1], "V_LD      ");
    adc_line(2,5);
    HEXtoBCD(ADC_Right_Detect >> 4);
    //strcpy(display_line[2], "V_RD      ");
    adc_line(3,5);
    display_changed = TRUE;
}
