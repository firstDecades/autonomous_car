//------------------------------------------------------------------------------
//
//  Description: This file contains the Main Routine - "While" Operating System
//
//  Jim Carlson
//  Jan 2023
//  Built with Code Composer Version: CCS12.4.0.00007_win64
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"

// Function Prototypes
void main(void);
void Init_Conditions(void);
void Display_Process(void);
void Init_LEDs(void);
void Carlson_StateMachine(void);
void Init_ADC(void);

  // Global Variables
volatile char slow_input_down;
/*
 * extern char display_line[4][11];
 * extern char *display[4];
*/
unsigned char display_mode;
/*
 * extern volatile unsigned char display_changed;
 * extern volatile unsigned char update_display;
 * extern volatile unsigned int update_display_count;
 * extern volatile unsigned int Time_Sequence;
 * extern volatile char one_time;
*/
unsigned int test_value;
char chosen_direction;
char change;

unsigned int wheel_move;
char forward;

unsigned int Last_Time_Sequence = 0;     //A variable to identify Time_Sequence has changed
unsigned int cycle_time = 0;             //is a new time base used to control making shapes
unsigned int time_change = 0;            //is an identifier that a change has occurred
volatile unsigned int Switch_State = NONE;           //identifies the state in switch state machine

volatile unsigned char IR_Status = OFF;

unsigned int Baudrate = 4;

extern unsigned int LEFT_WHITE = 0;
extern unsigned int RIGHT_WHITE = 0;

unsigned char Process_Buffer[4][34];
unsigned char Process_Buffer_USB[4][34];
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
//unsigned int IOT_tx_check = FALSE;
//unsigned int USB_tx_check = FALSE;

//void main(void){
void main(void){
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

//------------------------------------------------------------------------------
// Main Program
// This is the main routine for the program. Execution of code starts here.
// The operating system is Back Ground Fore Ground.
//
//------------------------------------------------------------------------------
  PM5CTL0 &= ~LOCKLPM5;
// Disable the GPIO power-on default high-impedance mode to activate
// previously configured port settings

  Last_Time_Sequence = 0;     //A variable to identify Time_Sequence has changed
  cycle_time = 0;             //is a new time base used to control making shapes
  time_change = 0;            //is an identifier that a change has occurred
  Switch_State = NONE;           //identifies the state in switch state machine

  unsigned int time_loop;
  Init_Port3(TRUE);
  Init_Ports(FALSE);                        // Initialize Ports
  Init_Clocks();                       // Initialize Clock System
  Init_Timers();                       // Initialize Timers
  Init_Conditions();                   // Initialize Variables and Initial Conditions

  while (ms200Time_Sequence < 10) {
      LCD_BACKLITE_DIMING = PERCENT_80;
      P6OUT &= ~GRN_LED;
  }

  Init_Port5(TRUE);
  Init_Port4(TRUE);
  Init_Port1(TRUE);

  Init_Port3(TRUE);

  LCD_BACKLITE_DIMING = WHEEL_OFF;
  P6OUT |= GRN_LED;
  Init_LCD();
  Init_ADC();
  Init_DAC();
  Init_Buffers();
  Init_Serial(4);
  Init_Process_Buffer();
  //Init_Port3(TRUE);                       //Set output as HIGH


  // Initialize LCD
//P2OUT &= ~RESET_LCD;
  // Place the contents of what you want on the display, in between the quotes
// Limited to 10 characters per line
  /*strcpy(display_line[0], "   NCSU   ");
  strcpy(display_line[1], " WOLFPACK ");
  strcpy(display_line[2], "  ECE306  ");
  strcpy(display_line[3], "  GP I/O  ");
  */
  strcpy(display_line[0], "          ");
  strcpy(display_line[1], "          ");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = TRUE;

  //Display_Update(0,0,0,0);

  wheel_move = 0;
  forward = TRUE;
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
  //P6OUT &= ~LCD_BACKLITE;
  LCD_BACKLITE_DIMING = WHEEL_OFF;
  ADC_Switch = SENSOR;


  /*
   * Left Detect: 140+ is black
   * Right Detect: 200 + is black
   */


  IR_Status = ON;
  P2OUT |= IR_LED;


//------------------------------------------------------------------------------
// Begining of the "While" Operating System
//------------------------------------------------------------------------------
  while(ALWAYS) {                       // Can the Operating system run

     P2OUT |= IR_LED;
    P6_LCheck();
    P6_RCheck();

    //LCD_update_ADC();
    LCD_update_Buadrate(); //line 3 Baudrate
    IOT_To_Buffer();
    USB_To_Buffer();
    if ((IOT_tx_check == FALSE) & (USB_tx_check == FALSE)) {
        tx_check = FALSE;
    }
    switch(rx_check) {
    case TRUE:
        rx_check = FALSE;
        switch (tx_check) {
        case FALSE:
            if (USB_rx_check == TRUE) {
                USB_rx_check = FALSE;
                Transmit_IOT(Process_Buffer);
            }
            if (IOT_rx_check == TRUE) {
                IOT_rx_check = FALSE;
                Transmit_PC(Process_Buffer);
            }
            break;
        default: break;
        }
        break;
    default: break;
    }
    /*
    if (IOT_2_PC[0] == '\0') {
        strcpy(display_line[0], " waiting  ");
        display_changed = TRUE;
    } else if (tx_buffer[0] == '\0') {
        strcpy(display_line[0], " received ");
        display_changed = TRUE;
    } else {
        strcpy(display_line[0], " transmit ");
        display_changed = TRUE;
    }
    */
    //HEXtoBCD(ADC_Thumb_Shift);
    //adc_line(2,6);
    TIMERtoBCD(ms200Time_Sequence);
    timer_line(4,5);
    //display_line[3][0] = Process_Buffer[0][0];
    //display_line[3][1] = Process_Buffer[0][1];
    //display_line[3][2] = Process_Buffer[0][2];
    //display_line[3][3] = Process_Buffer[0][3];
    //display_line[3][4] = Process_Buffer[0][4];
    //display_line[3][5] = Process_Buffer[0][5];
    //display_line[3][6] = Process_Buffer[0][6];
    //display_line[3][7] = Process_Buffer[0][7];
    display_changed = TRUE;

    Carlson_StateMachine();             // Run a Time Based State Machine
    Switches_Process();                 // Check for switch state change
    Display_Process();                  // Update Display
    P3OUT ^= TEST_PROBE;                // Change State of TEST_PROBE OFF
    if (Last_Time_Sequence != Time_Sequence) {  //update time
        Last_Time_Sequence = Time_Sequence;    //sync time
        cycle_time++;                           //increment the counter
        time_change = 1;                        //note that time has changed
    }


    switch(Switch_State) {
    //Wheel_Stop();
    case SWITCH_1:                       //straight
        switch(ADC_Thumb_Shift) {
        case 0:
            Baudrate = 4;
            //P1OUT ^= RED_LED;
            Init_Serial(4);
            Switch_State = NONE;
            break;
        case 1:
            Baudrate = 17;
            //P6OUT ^= GRN_LED;
            Init_Serial(17);
            Switch_State = NONE;
            break;
        case 2:
            LEFT_BLACK = ADC_Left_Shift;
            RIGHT_BLACK = ADC_Right_Shift;
            Switch_State = NONE;
            break;
        case 3:
            LEFT_WHITE = ADC_Left_Shift;
            RIGHT_WHITE = ADC_Right_Shift;
            Switch_State = NONE;
            break;

        case 6:
            direct_usb = BEGINNING;
            usb_rx_wr = BEGINNING;
            Switch_State = NONE;
            break;
         default: break;
        }
        //Wheels_Process();
        //LINE_CHECK();
        //ADC_Switch = THUMB;
        break;
    case SWITCH_2:                       //circle
        //ADC_Switch = SENSOR;
        //LINE_CHECK();
        //Wheel_Stop();
        //Switch_State = NONE;
        break;
    default:

        RIGHT_FORWARD_SPEED = WHEEL_OFF;
        LEFT_FORWARD_SPEED = WHEEL_OFF;
        RIGHT_REVERSE_SPEED = WHEEL_OFF;
        LEFT_REVERSE_SPEED = WHEEL_OFF;
        break;
    }

/*
        switch (debounce_switch) {
        case SWITCH_1:
            strcpy(display_line[0], " Switch 1 ");
            strcpy(display_line[1], " debounce ");
            display_changed = TRUE;
            break;
        case SWITCH_2:
            strcpy(display_line[0], " Switch 2 ");
            strcpy(display_line[1], " debounce ");
            display_changed = TRUE;
            break;
        case NONE:
            strcpy(display_line[0], "    No    ");
            strcpy(display_line[1], " Switches ");
            strcpy(display_line[2], "          ");
            display_changed = TRUE;
        default:
            break;
        }
        switch (debounce_count) {
        case 20:
            strcpy(display_line[2], "    20    ");
            display_changed = TRUE;
            break;
        case 80:
            strcpy(display_line[2], "    80    ");
            display_changed = TRUE;
            break;
        case 0:
            strcpy(display_line[2], "    00    ");
            display_changed = TRUE;
            break;
        default: break;
        }
        */
  }
//------------------------------------------------------------------------------

}





void Carlson_StateMachine(void){
    switch(Time_Sequence){
      case 250:                        //
        if(one_time){
          //Init_LEDs();
          //lcd_BIG_mid();
          display_changed = TRUE;
          one_time = 0;
        }
        Time_Sequence = 0;             //
        break;
      case 200:                        //
        if(one_time){
          //P1OUT &= ~RED_LED;            // Change State of LED 4
          //P6OUT |= GRN_LED;            // Change State of LED 5
          one_time = 0;
          //strcpy(display_line[0], "   NCSU   ");
          //strcpy(display_line[1], " WOLFPACK ");
          //strcpy(display_line[2], " state 200");
          //display_changed = TRUE;

          //P6OUT |= L_FORWARD;
          //P6OUT |= R_FORWARD;
        }
        break;
      case 150:                         //
        if(one_time){
         // P1OUT |= RED_LED;            // Change State of LED 4
          //P6OUT &= ~GRN_LED;            // Change State of LED 5
          one_time = 0;
          //strcpy(display_line[0], "   NCSU   ");
          //strcpy(display_line[1], " WOLFPACK ");
          //strcpy(display_line[2], " state 150");
          //display_changed = TRUE;

          //P6OUT &= ~L_FORWARD;
          //P6OUT &= ~R_FORWARD;
        }
        break;
      case 100:                         //
        if(one_time){
//          lcd_4line();
          //lcd_BIG_bot();
          //P6OUT |= GRN_LED;            // Change State of LED 5
          //P1OUT |= RED_LED;
          display_changed = 1;
          one_time = 0;
          //strcpy(display_line[0], "   NCSU   ");
          //strcpy(display_line[1], " WOLFPACK ");
          //strcpy(display_line[2], " state 100");
          //display_changed = TRUE;
        }
        break;
      case  50:                        //
        if(one_time){
          one_time = 0;
          //P1OUT &= ~RED_LED;
          //P6OUT &= ~GRN_LED;
          //strcpy(display_line[0], "   NCSU   ");
          //strcpy(display_line[1], " WOLFPACK ");
          //strcpy(display_line[2], "  state 50");
          //display_changed = TRUE;
        }
        break;                         //
      default: break;
    }
}

void P6_LCheck(void){
    if ((LEFT_FORWARD_SPEED != WHEEL_OFF) & (LEFT_REVERSE_SPEED != WHEEL_OFF)) {
        LEFT_FORWARD_SPEED = WHEEL_OFF;
        LEFT_REVERSE_SPEED = WHEEL_OFF;
    }
    /*if ((P6IN & L_FORWARD) && (P6IN & L_REVERSE)) {
        P6OUT &= ~L_FORWARD;
        P6OUT &= ~L_REVERSE;
        P1OUT |= RED_LED;
    }*/
}
void P6_RCheck(void){
    if ((RIGHT_FORWARD_SPEED != WHEEL_OFF) & (RIGHT_REVERSE_SPEED != WHEEL_OFF)) {
            RIGHT_FORWARD_SPEED = WHEEL_OFF;
            RIGHT_REVERSE_SPEED = WHEEL_OFF;
        }
    /*if ((P6IN & R_FORWARD) && (P6IN & R_REVERSE)) {
        P6OUT &= ~R_FORWARD;
        P6OUT &= ~R_REVERSE;
        P1OUT |= RED_LED;
    }*/
}

void Init_Process_Buffer(void) {
    int i = 0;
    int j = 0;
    for (i = 0; i < 4; i ++) {
        for (j = 0; j < 34; j++) {
            Process_Buffer[i][j] = NULL_CHAR;
            //Process_Buffer_USB[i][j] = NULL_CHAR;
        }
        //Process_Buffer[i][32] = CARRIAGE_RETURN;
        //Process_Buffer[i][33] = LINE_FEED;
    }
    direct_usb = 0;
    usb_rx_wr = 0;
}

void clear_row(unsigned char process_buf[4][34], int row) {
    unsigned int j = 0;
    for (j = 0; j < 34; j++) {
        process_buf[row][j] = NULL_CHAR;
    }
    //Process_Buffer[row][32] = CARRIAGE_RETURN;
    //Process_Buffer[row][33] = LINE_FEED;
}
void USB_To_Buffer(void) {
    if (usb_rx_rd != usb_rx_wr) {
        if (direct_usb >= 34) {
            direct_usb = BEGINNING;
            buffer_row ++;
            if (buffer_row > 3) {
                buffer_row = 0;
            }
            clear_row(Process_Buffer, buffer_row);
        }
        Process_Buffer[buffer_row][direct_usb] = PC_2_IOT[usb_rx_rd++];
        if (Process_Buffer[buffer_row][direct_usb++] == LINE_FEED) {
            rx_check = TRUE;
            buffer_row ++;
            direct_usb = BEGINNING;
            if (buffer_row > 3) {
                buffer_row = BEGINNING;
            }
            clear_row(Process_Buffer, buffer_row);
        }
        if (usb_rx_rd >= sizeof(PC_2_IOT)) {
            usb_rx_rd = BEGINNING;
        }
    }
    //rx_check = TRUE;
}
void IOT_To_Buffer(void) {
    if (iot_rx_rd != iot_rx_wr) {

        if (direct_iot >= 34) {
            direct_iot = BEGINNING;
            buffer_row++;
            if (buffer_row > 3) {
                buffer_row = 0;
            }
            clear_row(Process_Buffer, buffer_row);
        }
        Process_Buffer[buffer_row][direct_iot] = IOT_2_PC[iot_rx_rd++];
        if (Process_Buffer[buffer_row][direct_iot++] == LINE_FEED) {
            rx_check = TRUE;
            buffer_row++;
            direct_iot = BEGINNING;
            if (buffer_row > 3) {
                buffer_row = 0;
            }
            clear_row(Process_Buffer, buffer_row);
        }
        if (iot_rx_rd >= sizeof(IOT_2_PC)) {
            iot_rx_rd = BEGINNING;
        }
        //PC_2_IOT[direct_usb++] = NULL_CHAR;
    }
    //rx_check = TRUE;
}
void Transmit_IOT(unsigned char process_buf[4][34]) {
    unsigned int i = 0;
    tx_iot_index = BEGINNING;
    for (i = 0; i < 34; i++) {
        tx_iot[i] = NULL_CHAR;
        //tx_hold[i] = NULL_CHAR;
    }

    for (i = 0; i < sizeof(process_buf[0]); i ++) {
        //tx_buffer[i] = Process_Buffer[0][i];
        tx_iot[i] = process_buf[0][i];
        //tx_hold[i] = Process_Buffer[0][i];
        //Process_Buffer[0][i] = NULL_CHAR;
        if (tx_iot[i] == LINE_FEED) {
            i = sizeof(process_buf[0]);
        }
    }

    //direct_usb = 0;
    /*display_line[1][0] = tx_buffer[0];
    display_line[1][1] = tx_buffer[1];
    display_line[1][2] = tx_buffer[2];
    display_line[1][3] = tx_buffer[3];
    display_changed = TRUE;*/
    Intercept_Transmit(tx_iot);
    Move_Process_Buffer(process_buf);
    UCA0IE |= UCTXIE;
    //IOT_tx_check = FALSE;
}
void Transmit_PC(unsigned char process_buf[4][34]) {
    unsigned int i = 0;
    tx_index = BEGINNING;
    tx_iot_index = BEGINNING;
    for (i = 0; i < 34; i++) {
        tx_buffer[i] = NULL_CHAR;
        //tx_hold[i] = NULL_CHAR;
    }

    for (i = 0; i < sizeof(process_buf[0]); i ++) {
        tx_buffer[i] = process_buf[0][i];
        //tx_iot[i] = process_buf[0][i];
        //tx_hold[i] = Process_Buffer[0][i];
        //Process_Buffer[0][i] = NULL_CHAR;
        if (tx_buffer[i] == LINE_FEED) {
            i = sizeof(process_buf[0]);
        }
    }

    //direct_usb = 0;
    /*display_line[1][0] = tx_buffer[0];
    display_line[1][1] = tx_buffer[1];
    display_line[1][2] = tx_buffer[2];
    display_line[1][3] = tx_buffer[3];
    display_changed = TRUE;*/
    Intercept_Transmit(tx_buffer);
    Move_Process_Buffer(Process_Buffer);
    UCA1IE |= UCTXIE;
    //USB_tx_check = FALSE;
}

void Move_Process_Buffer(unsigned char process_buf[4][34]) {
    unsigned int i;
    unsigned int j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 34; j++) {
            process_buf[i][j] = process_buf[i+1][j];
        }
    }
    clear_row(process_buf, 3);
    if(buffer_row != 0) {
        buffer_row --;
    }
}

void Intercept_Transmit(unsigned char tx_hold[34]) {
    switch(tx_hold[0] == '^') {
        //if first char is a command
    case TRUE:
        switch(tx_hold[1]) {
        case 'F': //switch baudrate to 460,800
            Baudrate = 17;
            Init_Serial(Baudrate);
            break;
        case 'S': //switch baudrate to 115,200
            Baudrate = 4;
            Init_Serial(Baudrate);
        }
        break;
        default: break;
    }

}
