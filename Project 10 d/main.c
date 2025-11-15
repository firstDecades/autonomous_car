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
unsigned int REFERENCE = 100;
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

extern unsigned int LEFT_WHITE = 90;
extern unsigned int RIGHT_WHITE = 90;

unsigned char setMUX[14] = "AT+CIPMUX=1\r\n";
unsigned char setSERVER[22] = "AT+CIPSERVER=1,5000\r\n";
unsigned char getIP[11] = "AT+CIFSR\r\n";
unsigned int i = 0;
unsigned int start_ping = FALSE;


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
  ms200Time_Sequence = 0;
  while (ms200Time_Sequence < 50) {
      LCD_BACKLITE_DIMING = PERCENT_80;
      P6OUT &= ~GRN_LED;
  }
  LCD_BACKLITE_DIMING = WHEEL_OFF;
  P6OUT |= GRN_LED;
  for (i = 0; i < sizeof(setMUX); i++) {
      tx_iot[i] = setMUX[i];
  }
  tx_iot_index = 0;
  IOT_tx_check = TRUE;
  UCA0IE |= UCTXIE;
  while (IOT_tx_check == TRUE) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  ms200Time_Sequence = 0;
  while (ms200Time_Sequence < 10) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  for (i = 0; i < sizeof(setSERVER); i++) {
      tx_iot[i] = setSERVER[i];
  }
  tx_iot_index = 0;
  IOT_tx_check = TRUE;
  UCA0IE |= UCTXIE;
  while (IOT_tx_check == TRUE) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  ms200Time_Sequence = 0;
  while (ms200Time_Sequence < 10) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  for (i = 0; i < sizeof(getIP); i++) {
      tx_iot[i] = getIP[i];
  }
  tx_iot_index = 0;
  getIP_check = FALSE;
  collectIP = FALSE;
  displayIP = FALSE;
  IOT_tx_check = TRUE;

  UCA0IE |= UCTXIE;
  while (IOT_tx_check == TRUE) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  ms200Time_Sequence = 0;
  while (ms200Time_Sequence < 10) {
      P6OUT |= GRN_LED;
  }
  P6OUT &= ~GRN_LED;
  while (displayIP != TRUE) {
      P6OUT |= GRN_LED;
  }
  P6OUT |= GRN_LED;
  for (i = 2; i < 12; i++) {
      display_line[0][i-2] = ip_string[i];
      display_line[1][i-2] = ip_string[i+10];
  }
  display_changed = TRUE;
  start_ping = TRUE;


  // Initialize LCD
//P2OUT &= ~RESET_LCD;
  // Place the contents of what you want on the display, in between the quotes
// Limited to 10 characters per line
  /*strcpy(display_line[0], "   NCSU   ");
  strcpy(display_line[1], " WOLFPACK ");
  strcpy(display_line[2], "  ECE306  ");
  strcpy(display_line[3], "  GP I/O  ");
  */
  //strcpy(display_line[0], "          ");
  //strcpy(display_line[1], "          ");
  //strcpy(display_line[2], "          ");
  //strcpy(display_line[3], "          ");
  //display_changed = TRUE;

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
      //RIGHT_FORWARD_SPEED = RIGHT_FAST;
      //LEFT_FORWARD_SPEED = LEFT_FAST;
     P2OUT |= IR_LED;
    P6_LCheck();
    P6_RCheck();

    LCD_update_ADC();
    //LCD_update_Buadrate(); //line 3 Baudrate
    IOT_To_Buffer();
    USB_To_Buffer();
    Run_RTC_Scheduler();

    if ((buffer_row != 0) && (USB_tx_check == FALSE) && (USB_toggle == FALSE)) {
        Transmit_PC();
    }
    if ((buffer_row_usb) != 0 && (IOT_tx_check == FALSE) && (IOT_toggle == FALSE)) {
        Transmit_IOT();
    }

    switch(Switch_State) {
    case SWITCH_1:
        time_change = 1;
        LEFT_BLACK = ADC_Left_Shift - 200;
        RIGHT_BLACK = ADC_Right_Shift - 200;
        LEFT_BLACK = (LEFT_BLACK - LEFT_WHITE) >> 1;
        RIGHT_BLACK = (RIGHT_BLACK - RIGHT_WHITE) >> 1;
        if (((int)RIGHT_BLACK - (int)LEFT_BLACK) < 0) {
            REFERENCE = LEFT_BLACK - RIGHT_BLACK;
        } else {
            REFERENCE = RIGHT_BLACK - LEFT_BLACK;
        }
        Switch_State = NONE;
        break;
    case SWITCH_2:
        LEFT_WHITE = ADC_Left_Shift;
        RIGHT_WHITE = ADC_Right_Shift;
        Switch_State = NONE;
        //Line_Following();
        //Wheel_Control();
        break;
    case BLACK:
        Wheels_Process();
        break;
    }
    //LINE_CHECK();


    //HEXtoBCD(ADC_Thumb_Shift);
    //adc_line(2,6);
    //TIMERtoBCD(ms200Time_Sequence);
    //timer_line(4,5);

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


