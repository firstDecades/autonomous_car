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

    if (buffer_row != 0 && (USB_tx_check == FALSE)) {
        Transmit_PC();
    }
    if (buffer_row_usb != 0 && (IOT_tx_check == FALSE)) {
        Transmit_IOT();
    }

    /*if ((IOT_tx_check == FALSE) & (USB_tx_check == FALSE)) {
        tx_check = FALSE;
    }

    switch(USB_toggle) {
    case TRUE:
        USB_toggle = FALSE;
        Transmit_PC(Process_Buffer_USB, &buffer_row_usb);
        break;
    default: break;
    }
    switch(IOT_toggle) {
    case TRUE:
        IOT_toggle = FALSE;
        Transmit_IOT(Process_Buffer, &buffer_row);
        break;
    default: break;
    }
    switch(rx_check) {
    case TRUE:
        rx_check = FALSE;
        switch (tx_check) {
        case FALSE:
            if (USB_rx_check == TRUE) {
                USB_rx_check = FALSE;
                Transmit_IOT(Process_Buffer, &buffer_row);
            }
            if (IOT_rx_check == TRUE) {
                IOT_rx_check = FALSE;
                Transmit_PC(Process_Buffer_USB, &buffer_row_usb);
            }
            break;
        default: break;
        }
        break;
    default: break;
    }
    */
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

        //RIGHT_FORWARD_SPEED = WHEEL_OFF;
        //LEFT_FORWARD_SPEED = WHEEL_OFF;
        //RIGHT_REVERSE_SPEED = WHEEL_OFF;
        //LEFT_REVERSE_SPEED = WHEEL_OFF;
        break;
    }
  }
//------------------------------------------------------------------------------

}





void Carlson_StateMachine(void){
    switch(Time_Sequence){
      case 250:                        //
        if(one_time){
          //lcd_BIG_mid();
          display_changed = TRUE;
          one_time = 0;
        }
        Time_Sequence = 0;             //
        break;
      case 200:                        //
        if(one_time){
          one_time = 0;
        }
        break;
      case 150:                         //
        if(one_time){
          one_time = 0;
        }
        break;
      case 100:                         //
        if(one_time) {
//          lcd_4line();
          //lcd_BIG_bot();
          one_time = 0;
        }
        break;
      case 50:                        //
        if(one_time){
          one_time = 0;
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
}
void P6_RCheck(void){
    if ((RIGHT_FORWARD_SPEED != WHEEL_OFF) & (RIGHT_REVERSE_SPEED != WHEEL_OFF)) {
            RIGHT_FORWARD_SPEED = WHEEL_OFF;
            RIGHT_REVERSE_SPEED = WHEEL_OFF;
        }
}


