/*
 * UART.c
 *
 *  Created on: Mar 24, 2025
 *      Author: chait
 *      description: initialize and set up Serial Communications
 */

#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

#define BEGINNING (0x00) //start of IOT_2_PC
//global
volatile unsigned char IOT_2_PC[32] = "\0";
volatile unsigned char PC_2_IOT[32] = "\0";
volatile unsigned int iot_rx_wr = BEGINNING;
volatile unsigned int iot_rx_rd = BEGINNING;
volatile unsigned int usb_rx_rd = BEGINNING;
volatile unsigned int usb_rx_wr = BEGINNING;
volatile unsigned int direct_iot = BEGINNING;
volatile unsigned int direct_usb = BEGINNING;
volatile unsigned int tx_index = BEGINNING;
volatile unsigned int tx_iot_index = BEGINNING;
unsigned int IOT_tx_check = FALSE;
unsigned int USB_tx_check = FALSE;
unsigned int command_count = 0;
unsigned int command_rx = FALSE;

/*
void clear_row(int row) {
    for (int j = 0; j < 34; j++) {
        Process_Buffer[row][j] = '\0';
    }
    Process_Buffer[i][32] = CARRIAGE_RETURN;
    Process_Buffer[i][33] = LINE_FEED;
}
for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 34; j++) {
        Process_Buffer[i][j] = '\0';
    }
    Process_Buffer[i][32] = CARRIAGE_RETURN;
    Process_Buffer[i][33] = LINE_FEED;
}


if (usb_rx_wr != usb_rx_rd) {
    usb_rx_rd ++;
    if (Process_Buffer[buffer_row][usb_rx_rd] == 0x0D) {
        buffer_row++;
        usb_rx_rd = BEGINNING;
        if (buffer_row > 3) {
            buffer_row = BEGINNING;
        }
        clear_row(buffer_row);
    }
    Process_Buffer[buffer_row][usb_rx_rd] = PC_2_IOT[usb_rx_wr];
}
*/

void Init_Serial(int speed) {
    Init_Serial_UCA0(speed);
    Init_Serial_UCA1(speed);
}
void Init_Buffers(void) {
    unsigned int i = 0;
    for(i = 0; i < sizeof(IOT_2_PC); i++) {
        IOT_2_PC[i] = NULL_CHAR;
        PC_2_IOT[i] = NULL_CHAR;
    }
    iot_rx_rd = BEGINNING;
    iot_rx_wr = BEGINNING;
    usb_rx_rd = BEGINNING;
    usb_rx_wr = BEGINNING;
    direct_usb = BEGINNING;
    direct_iot = BEGINNING;
}


void Init_Serial_UCA0 (int speed) {
    //------------------------------------------------------------------------------
    //                                                      TX error (%)    RX error (%)
    // BRCLK    Baudrate    UCOS16  UCBRx   UCFx    UCSx    neg     pos     neg     pos
    // 8000000  4800        1       104     2       0xD6    -0.08   0.04    -0.10   0.14
    // 8000000  9600        1       52      1       0x49    -0.08   0.04    -0.10   0.14
    // 8000000  19200       1       26      0       0xB6    -0.08   0.16    -0.28   0.20
    // 8000000  57600       1       8       10      0xF7    -0.32   0.32    -1.00   0.36
    // 8000000  115200      1       4       5       0x55    -0.80   0.64    -1.12   1.76
    // 8000000  460800      0       17      0       0x4A    -2.72   2.56    -3.76   7.28
    //------------------------------------------------------------------------------
    //Configure eUSCI_A0 for UART mode
    UCA0CTLW0 = 0;
    UCA0CTLW0 |= UCSWRST;   //Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;    //set SMCLK as fBRCLK
    UCA0CTLW0 &= ~UCMSB;            //MSB, LSB select, selecting LSB
    UCA0CTLW0 &= ~UCPEN;            //No parity
    UCA0CTLW0 &= ~UCSYNC;           //Asynchronous
    UCA0CTLW0 &= ~UC7BIT;           //8 bit mode
    UCA0CTLW0 |= UCMODE_0;          //
    // BRCLK    Baudrate    UCOS16  UCBRx   UCFx    UCSx    neg     pos     neg     pos
    // 8000000  115200      1       4       5       0x55    -0.80   0.64    -1.12   1.76
    // UCA?MCTLW = UCSx + UCFx + UCOS16
    // UCA?MCTLW = 0x55 + 5 + 1 = 0x5551 - 115,200
    // UCA?MCTLW = 0x4A + 1 + 0 = 0x4A10 - 460,800
    switch(speed) {
    case 4:
        UCA0BRW = 4;
        UCA0MCTLW = 0x5551;
        break;
    case 17:
        UCA0BRW = 17;
        UCA0MCTLW = 0x4A00;
        break;
    default: break;
    }
    //UCA0BRW = 4;                    //115,200 baud
    //UCA0MCTLW = 0x5551;               //

    UCA0CTLW0 &= ~UCSWRST;          //release from reset
    //UCA0TXBUF = 0x00;               //Prime the Pump
    UCA0IE |= UCRXIE;               //Enable RX interrupt
}
void Init_Serial_UCA1 (int speed) {
    //------------------------------------------------------------------------------
    //                                                      TX error (%)    RX error (%)
    // BRCLK    Baudrate    UCOS16  UCBRx   UCFx    UCSx    neg     pos     neg     pos
    // 8000000  4800        1       104     2       0xD6    -0.08   0.04    -0.10   0.14
    // 8000000  9600        1       52      1       0x49    -0.08   0.04    -0.10   0.14
    // 8000000  19200       1       26      0       0xB6    -0.08   0.16    -0.28   0.20
    // 8000000  57600       1       8       10      0xF7    -0.32   0.32    -1.00   0.36
    // 8000000  115200      1       4       5       0x55    -0.80   0.64    -1.12   1.76
    // 8000000  460800      0       17      0       0x4A    -2.72   2.56    -3.76   7.28
    //------------------------------------------------------------------------------
    //Configure eUSCI_A1 for UART mode
    UCA1CTLW0 = 0;
    UCA1CTLW0 |= UCSWRST;   //Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK;    //set SMCLK as fBRCLK
    UCA1CTLW0 &= ~UCMSB;            //MSB, LSB select, selecting LSB
    UCA1CTLW0 &= ~UCPEN;            //No parity
    UCA1CTLW0 &= ~UCSYNC;           //
    UCA1CTLW0 &= ~UC7BIT;           //8 bit mode
    UCA1CTLW0 |= UCMODE_0;          //
    // BRCLK    Baudrate    UCOS16  UCBRx   UCFx    UCSx    neg     pos     neg     pos
    // 8000000  115200      1       4       5       0x55    -0.80   0.64    -1.12   1.76
    // UCA?MCTLW = UCSx + UCFx + UCOS16
    // UCA?MCTLW = 0x55 + 5 + 1 = 0x5551 - 115,200
    // UCA?MCTLW = 0x4A + 1 + 0 = 0x4A10 - 460,800
    switch(speed) {
    case 4:
        UCA1BRW = 4;
        UCA1MCTLW = 0x5551;
        break;
    case 17:
        UCA1BRW = 17;
        UCA1MCTLW = 0x4A00;
        break;
    default: break;
    }
    //UCA1BRW = 4;                    //115,200 baud
    //UCA1MCTLW = 0x5551;               //

    UCA1CTLW0 &= ~UCSWRST;          //release from reset
    //UCA1TXBUF = 0x00;               //Prime the Pump
    UCA1IE |= UCRXIE;               //Enable RX interrupt
}



//interrupt for RX - TX, UCA0
#pragma vector = EUSCI_A0_VECTOR

__interrupt void eUSCI_A0_ISR(void) {
    unsigned int temp;
    switch(__even_in_range(UCA0IV, 0x08)) {
    case 0:
        //vector 0: no interrupt
        break;
    case 2:
        //Vector 2: RXIFG
        //code for Receive
        temp = iot_rx_wr++;
        IOT_rx_check = TRUE;
        //P6OUT ^= GRN_LED;
        IOT_2_PC[temp] = UCA0RXBUF;     //Rx -> IOT_2_PC character array
        if (IOT_2_PC[temp] == '^') {
            command_rx = TRUE;
            command_count = 0;
            P6OUT ^= GRN_LED;
        }
        command_count++;
        if (command_rx == TRUE) {
            if (command_count >= 10) {
                command_rx = FALSE;
                command_count = 0;
                if (temp+1 >= sizeof(IOT_2_PC)) {
                    IOT_2_PC[0] = CARRIAGE_RETURN;
                    IOT_2_PC[1] = LINE_FEED;
                    iot_rx_wr = 2;
                } else if (temp+2 >= sizeof(IOT_2_PC)) {
                    IOT_2_PC[temp+1] = CARRIAGE_RETURN;
                    IOT_2_PC[0] = LINE_FEED;
                    iot_rx_wr = 1;
                }
                else {
                    IOT_2_PC[temp+1] = CARRIAGE_RETURN;
                    IOT_2_PC[temp+2] = LINE_FEED;
                    iot_rx_wr += 2;
                }
            }
        } else {
            iot_rx_wr --;
            UCA1TXBUF = IOT_2_PC[temp];
        }
        if (iot_rx_wr >= (sizeof(IOT_2_PC))) {
            iot_rx_wr = BEGINNING;      //Circular buffer back to beginning
        }
        //UCA1TXBUF = IOT_2_PC[temp];
        //UCA1IE |= UCTXIE;
        //UCA0IE |= UCTXIE;
        //UCA0TXBUF = IOT_2_PC[temp];     //transmit out the same port
        break;
    case 4:
        //Vector 4: TXIFG
        //Code for Transmit
        //break;
        IOT_tx_check = TRUE;
        temp = tx_iot_index++; //increment tx_iot_index
        UCA0TXBUF = tx_iot[temp]; //transmit tx_iot
        if (tx_iot[temp] == NULL_CHAR) { //if char was null, stop
            //allows for one null char to go through, good for iot connection?
            UCA0IE &= ~UCTXIE;
            IOT_tx_check = FALSE;
        }
        if (tx_iot[temp] == LINE_FEED) { //if char was line feed, stop
            UCA0IE &= ~UCTXIE;
            IOT_tx_check = FALSE;
        } else if (tx_iot_index >= sizeof(tx_iot)) { //if at end of array, stop
            UCA0IE &= ~UCTXIE;
            IOT_tx_check = FALSE;
            IOT_toggle = TRUE;
            //at end of array but no end line feed, so keep looping
        }
        tx_iot[temp] = NULL_CHAR; //null char to prevent looping

        break;
    default: break;
    }
}

//interrupt for RX-TX UCA1
#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
    unsigned int temp;
    //----------------
    //Echo back RXed character, confirm TX buffer is ready first
    switch(__even_in_range(UCA1IV, 0x08)) {
    case 0:
        //vector 0: no interrupt
        break;
    case 2:
        //vector 2: RXIFG
        //P1OUT ^= RED_LED;
        //temp = usb_rx_wr;
        temp = usb_rx_wr++;
        //rx_check = TRUE;
        USB_rx_check = TRUE;
        PC_2_IOT[temp] = UCA1RXBUF;         //RX -> PC_2_IOT character array
        if (usb_rx_wr >= (sizeof(IOT_2_PC))) {
            usb_rx_wr = BEGINNING;      //Circular buffer back to beginning
        }

        //UCA0IE |= UCTXIE;                   //Enable TX interrupt
        //UCA1TXBUF = PC_2_IOT[temp];
        break;
    case 4:
        //vector 4: TXIFG
        //break;
        USB_tx_check = TRUE;
        temp = tx_index;
        if (tx_index < 5) {
            display_line[1][tx_index] = tx_buffer[tx_index];
            display_changed = TRUE;
        }
        UCA1TXBUF = tx_buffer[tx_index++];

        //UCA1TXBUF = PC_2_IOT[direct_iot++];

        if (tx_buffer[temp] == NULL_CHAR) { //if char was null, stop
            //allows for one null char to go through, good for iot connection?
            UCA1IE &= ~UCTXIE;
            USB_tx_check = FALSE;
        }
        if (tx_buffer[temp] == LINE_FEED) {
            UCA1IE &= ~UCTXIE;
            USB_tx_check = FALSE;
        } else if (tx_index >= (sizeof(tx_buffer))) {//end of array, stop
            UCA1IE &= ~UCTXIE;
            USB_tx_check = FALSE;
            USB_toggle = TRUE;
            //end of array but not end line feed, keep transmitting
        }
        tx_buffer[temp] = NULL_CHAR;
        //if (tx_index >= (sizeof(PC_2_IOT))) {
        //    direct_iot = BEGINNING;
        //}
        //if (direct_iot == usb_rx_wr) {
        //    UCA0IE &= ~UCTXIE;              //Disable TX interrupt
        //}
        break;
    default: break;
    }
}

void LCD_update_Buadrate(void) {
    switch(Baudrate) {
    case 4:
        strcpy(display_line[2], "br: 115200");
        display_changed = TRUE;
        break;
    case 17:
        strcpy(display_line[2], "br: 460800");
        display_changed = TRUE;
        break;
    default:
        break;

    }
}
