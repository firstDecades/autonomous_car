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

//Global Variables
unsigned char Process_Buffer[MAX_ROWS][34]; //Process Buffer for IOT receives
unsigned char Process_Buffer_USB[MAX_ROWS][34]; //Process Buffer for USB receives -- not used in IOT commands
unsigned char tx_buffer[34] = "\0"; //Transmit buffer for USB TX
unsigned char tx_iot[34] = "\0"; //Transmit buffer for IOT TX
unsigned int buffer_row = 0; //control for Process_Buffer
unsigned int buffer_row_usb = BEGINNING; //control for Process_Buffer_USB
unsigned char NCSU_tx[9] = "NCSU  #1"; //HW text
unsigned int ncsuLoop = 0; //HW text index
//unsigned int rx_check = FALSE; //unused
//unsigned int tx_check = FALSE; //unused
//unsigned int USB_rx_check = FALSE; //unused
//unsigned int IOT_rx_check = FALSE; //unused
unsigned int IOT_toggle = FALSE; //used in IOT to Buffer for Transmit check
unsigned int USB_toggle = FALSE; //used in USB to Buffer for Transmit check


/*
 * Init_Process_Buffer(void)
 * arguments: void
 * initialize process buffers with Null Character
 * reset all buffer variables for iteration, including ring buffer variables (does not reset ring buffer values, just indexes)
 */
void Init_Process_Buffer(void) {
    int i = 0;
    int j = 0;
    //clear both process buffers
    for (i = 0; i < MAX_ROWS; i ++) {
        for (j = 0; j < sizeof(Process_Buffer[0]); j++) {
            Process_Buffer[i][j] = NULL_CHAR;
            Process_Buffer_USB[i][j] = NULL_CHAR;
        }
    }
    //reset all variables to beginning
    direct_usb = 0;
    direct_iot = 0;
    iot_rx_wr = 0;
    iot_rx_rd = 0;
    usb_rx_rd = 0;
    usb_rx_wr = 0;
    buffer_row = 0;
    buffer_row_usb = 0;
}

/*
 * clear_row(unsigned char process_buf[MAX_ROWS][34], int row)
 * arguments: process_buf[][]->allows both process buffers to be reset, row->location of row to be reset
 * given a process buffer and row, change all values in row to Null Char
 * does not affect buffer_row or buffer_row_usb, as which process buffer is not identified
 */
void clear_row(unsigned char process_buf[MAX_ROWS][34], int row) {
    unsigned int j = 0;
    //for char in
    for (j = 0; j < sizeof(process_buf[0]); j++) {
        process_buf[row][j] = NULL_CHAR;
    }
}

/*
 * USB_To_Buffer(void)
 * Arguments: void
 * specifically for Process_Buffer_USB
 * Stores information from UART A1 in Process Buffer
 * if usb_rx_wr was updated in UART.c, update Process Buffer USB
 * if direct_usb reaches the end of the row, move to next row and keep recording
 * if new char is a LINE FEED, move to next row
 */
void USB_To_Buffer(void) {
    //if buffer row exceeds MAX_ROWS, reset to beginning
    if (buffer_row_usb >= MAX_ROWS) {
        buffer_row_usb = BEGINNING;
    }
    //check for updates
    if (usb_rx_rd != usb_rx_wr) { //usb is UCA1 rx
        USB_toggle = TRUE; //if updating, turn on toggle, prevents Transmit
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
    else { //if not updating, turn off toggle, allows Transmit
        USB_toggle = FALSE;
    }
}

/*
 * IOT_To_Buffer(void)
 * Arguments: void
 * specifically for Process_Buffer
 * Stores information from UART A0 in Process Buffer
 * if iot_rx_wr was updated in UART.c, update Process Buffer
 * if direct_iot reaches the end of the row, move to next row and keep recording
 * if new char is a LINE FEED, move to next row
 */
void IOT_To_Buffer(void) {
    //if buffer row reaches or exceeds MAX ROWS, reset to beginning
    if (buffer_row >= MAX_ROWS) {
        buffer_row = BEGINNING;
    }
    if (iot_rx_rd != iot_rx_wr) { //iot is UCA0 rx
        IOT_toggle = TRUE; //turn on toggle, prevents Transmit
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
    } else { //if not updating, turn off toggle, allows Transmit
        IOT_toggle = FALSE;
    }
}

/*
 * Transmit_IOT(void)
 * Arguments: void
 * specifically for tx_iot and Process_Buffer_USB
 * information in Process_Buffer_USB will be sent to IOT
 */
void Transmit_IOT(void) {
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

/*
 * Transmit_PC(void)
 * Arguments: void
 * specifically for tx_buffer and Process_Buffer
 * information in Process_Buffer will be sent to PC
 */
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

/*
 * Move_Process_Buffer_USB(void)
 * Arguments: void
 * specifically for Process_Buffer_USB
 * move all rows one down.
 * update buffer_row_usb.
 */
void Move_Process_Buffer_USB(void) {
  unsigned int i = 0;
  unsigned int j = 0;
  clear_row(Process_Buffer_USB, 0); //clear the first row
  for (i = 0; i < MAX_ROWS-1; i++) { //move all rows down one
      for (j = 0; j < sizeof(Process_Buffer_USB[0]); j++ ) {
        Process_Buffer_USB[i][j] = Process_Buffer_USB[i+1][j];
      }
    }
    clear_row(Process_Buffer_USB, MAX_ROWS-1); //clear final row
    if (buffer_row_usb > 0) {
        buffer_row_usb --;
  }
}

/*
 * Move_Process_Buffer(void)
 * Arguments: void
 * specifically for Process_Buffer
 * move all rows one down.
 * update buffer_row.
 */
void Move_Process_Buffer(void) {
  unsigned int i = 0;
  unsigned int j = 0;
  clear_row(Process_Buffer, 0); //clear the first row
  for (i = 0; i < MAX_ROWS-1; i++) { //move all rows down one
      for (j = 0; j < sizeof(Process_Buffer[0]); j++ ) {
        Process_Buffer[i][j] = Process_Buffer[i+1][j];
      }
    }
    clear_row(Process_Buffer, MAX_ROWS-1); //clear final row
    if (buffer_row > 0) {
        buffer_row --;
  }
}

/*
 * Intercept_Transmit(unsigned char tx_hold[34])
 * Arguments: tx_hold[34] -> tx_iot or tx_buffer, allows for both to be checked
 * determine if outgoing transmit was a command
 * if so, turn on Password Check and add task
 */
void Intercept_Transmit(unsigned char tx_hold[34]) {
    unsigned int password_check = FALSE;
    switch(tx_hold[0] == '^') {
        //if first char is a command
    case TRUE:
        //password check
        if (tx_hold[1] == '1' && tx_hold[2] == '0' && tx_hold[3] == '2' && tx_hold[4] == '7') {
            password_check = TRUE;
        }
        else {//if not a password, turn off password_check
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

/*
 * String_To_Num(unsigned char string[4])
 * Arguments: string[4]->string of num
 * string will be converted to a number, decimal
 * type: quarter seconds
 */
int String_To_Num(unsigned char string[4]) {
    unsigned int num = (string[0] - (0x30))*1000;
    num += (string[1] - (0x30))*100;
    num += (string[2] - (0x30))*10;
    num += (string[3] - (0x30));
    return num;
}
