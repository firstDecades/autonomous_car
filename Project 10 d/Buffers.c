/*
 * Buffers.c
 *
 *  Created on: Apr 16, 2025
 *      Author: chait
 */

#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

unsigned char Process_Buffer[MAX_ROWS][34];
unsigned char Process_Buffer_USB[MAX_ROWS][34];
unsigned char tx_buffer[34] = "\0";
unsigned char tx_iot[34] = "\0";
unsigned int buffer_row = 0;
unsigned int buffer_row_usb = BEGINNING;
unsigned char NCSU_tx[9] = "NCSU  #1";
unsigned int ncsuLoop = 0;
unsigned int rx_check = FALSE;
unsigned int tx_check = FALSE;
unsigned int USB_rx_check = FALSE;
unsigned int IOT_rx_check = FALSE;
unsigned int IOT_toggle = FALSE;
unsigned int USB_toggle = FALSE;



void Init_Process_Buffer(void) {
    int i = 0;
    int j = 0;
    for (i = 0; i < MAX_ROWS; i ++) {
        for (j = 0; j < sizeof(Process_Buffer[0]); j++) {
            Process_Buffer[i][j] = NULL_CHAR;
            Process_Buffer_USB[i][j] = NULL_CHAR;
        }
    }
    direct_usb = 0;
    direct_iot = 0;
    iot_rx_wr = 0;
    iot_rx_rd = 0;
    usb_rx_rd = 0;
    usb_rx_wr = 0;
}

void clear_row(unsigned char process_buf[MAX_ROWS][34], int row) {
    unsigned int j = 0;
    for (j = 0; j < sizeof(process_buf); j++) {
        process_buf[row][j] = NULL_CHAR;
    }
}


void USB_To_Buffer(void) {
    if (buffer_row_usb >= MAX_ROWS) {
        buffer_row_usb = MAX_ROWS-1;
    }
    if (usb_rx_rd != usb_rx_wr) { //usb is UCA1 rx
        USB_toggle = TRUE;
        if (direct_usb >= sizeof(Process_Buffer_USB[0])) { //if process buffer counter is past array
            direct_usb = BEGINNING; //set counter to beginning
            buffer_row_usb ++; //move array row up by 1
            if (buffer_row_usb > MAX_ROWS-1) { //if exceeds bounds
                buffer_row_usb = BEGINNING; //reset to zero
              }
              clear_row(Process_Buffer_USB, buffer_row_usb); //clear the new row
            }
            Process_Buffer_USB[buffer_row_usb][direct_usb] = PC_2_IOT[usb_rx_rd++];
            //add to new char to Process_Buffer
            if (Process_Buffer_USB[buffer_row_usb][direct_usb++] == LINE_FEED) { //if new char is line feed (end of transmission)
              buffer_row_usb ++; //move to next line
              direct_usb = BEGINNING; //set counter to beginnning
              if (buffer_row_usb > MAX_ROWS-1) {// if exceeds bounds
                  buffer_row_usb = BEGINNING;
                }
                clear_row(Process_Buffer_USB, buffer_row_usb);
        }
        if (usb_rx_rd >= sizeof(PC_2_IOT)) {
            usb_rx_rd = BEGINNING;
      }
    }
    else {
        USB_toggle = FALSE;
    }
}

void IOT_To_Buffer(void) {
    if (buffer_row >= MAX_ROWS) {
        buffer_row = MAX_ROWS-1;
    }
    if (iot_rx_rd != iot_rx_wr) { //iot is UCA0 rx
        IOT_toggle = TRUE;
        if (direct_iot >= sizeof(Process_Buffer[0])) { //if past array
            direct_iot = BEGINNING; //set to beginning
            buffer_row ++; //move to next row
            if (buffer_row > MAX_ROWS-1) { //loop to beginning if at end
                buffer_row = BEGINNING;
            }
            clear_row(Process_Buffer, buffer_row); //clear new row
        }
        Process_Buffer[buffer_row][direct_iot] = IOT_2_PC[iot_rx_rd++]; //add to array
        if (Process_Buffer[buffer_row][direct_iot++] == LINE_FEED) { //if new char is line feed
            buffer_row++; //move to next row
            direct_iot = BEGINNING; //move to beginning of array
            if (buffer_row > MAX_ROWS-1) { //loop to beginning if at end
                buffer_row = BEGINNING;
            }
            clear_row(Process_Buffer, buffer_row); //clear new row
        }
        if (iot_rx_rd >= sizeof(IOT_2_PC)) { //if counter exceeds bounds
            iot_rx_rd = BEGINNING; //set to beginning
        }
    } else {
        IOT_toggle = FALSE;
    }
}






void Transmit_IOT() {
  unsigned int i = 0;
  tx_iot_index = BEGINNING; //set index to beginning
  for (i = 0; i < sizeof(tx_iot); i++) { //clear tx_iot
    tx_iot[i] = NULL_CHAR;
  }

  for (i = 0; i < sizeof(Process_Buffer_USB[0]); i++) {
    tx_iot[i] = Process_Buffer_USB[0][i]; //move Process_Buffer_USB (A1 info) into tx_iot
  }

  Intercept_Transmit(tx_iot);
  Move_Process_Buffer_USB();
  UCA0IE |= UCTXIE;
}


void Transmit_PC(void) {
  unsigned int i = 0;
  tx_index = BEGINNING; //set index to start
  for (i = 0; i < sizeof(tx_buffer); i ++) {
      tx_buffer[i] = NULL_CHAR; //clear buffer
  }

  for (i = 0; i < sizeof(Process_Buffer[0]); i++) {
      tx_buffer[i] = Process_Buffer[0][i];
    }
    Intercept_Transmit(tx_buffer);
    Move_Process_Buffer();
    UCA1IE |= UCTXIE;
}

void Move_Process_Buffer_USB(void) {
  unsigned int i = 0;
  unsigned int j = 0;
  clear_row(Process_Buffer_USB, 0);
  for (i = 0; i < MAX_ROWS-1; i++) {
      for (j = 0; j < sizeof(Process_Buffer_USB[0]); j++ ) {
        Process_Buffer_USB[i][j] = Process_Buffer_USB[i+1][j];
      }
    }
    clear_row(Process_Buffer_USB, MAX_ROWS-1);
    if (buffer_row_usb > 0) {
        buffer_row_usb --;
  }
}

void Move_Process_Buffer(void) {
  unsigned int i = 0;
  unsigned int j = 0;
  clear_row(Process_Buffer, 0);
  for (i = 0; i < MAX_ROWS-1; i++) {
      for (j = 0; j < sizeof(Process_Buffer[0]); j++ ) {
        Process_Buffer[i][j] = Process_Buffer[i+1][j];
      }
    }
    clear_row(Process_Buffer, MAX_ROWS-1);
    if (buffer_row > 0) {
        buffer_row --;
  }
}

void Intercept_Transmit(unsigned char tx_hold[34]) {
    unsigned int password_check = FALSE;
    switch(tx_hold[0] == '^') {
        //if first char is a command
    case TRUE:
        if (tx_hold[1] == '1' && tx_hold[2] == '0' && tx_hold[3] == '2' && tx_hold[4] == '7') {
            password_check = TRUE;
        }
        else {
            password_check = FALSE;
        }
        break;
    default: break;
    }

    if (password_check == TRUE) {
        unsigned char string[4] = {tx_hold[6], tx_hold[7], tx_hold[8], tx_hold[9]};
        unsigned int num = String_To_Num(string);
        add_task(tx_hold[5], (num*2)); //2*period to have per half second
        //add_task(tx_hold[5], num);  //period is per quarter second
    }

}

int String_To_Num(unsigned char string[4]) {
    unsigned int num = (string[0] - (0x30))*1000;
    num += (string[1] - (0x30))*100;
    num += (string[2] - (0x30))*10;
    num += (string[3] - (0x30));
    return num;
}
