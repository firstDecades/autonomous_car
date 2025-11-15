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

unsigned int fakeVariable =0;

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


  Init_Ports();                        // Initialize Ports
  Init_Clocks();                       // Initialize Clock System
  Init_Conditions();                   // Initialize Variables and Initial Conditions
  Init_Timers();                       // Initialize Timers
  Init_LCD();
  Init_ADC();

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
  display_line[1][9] = 'N';
  display_changed = TRUE;


//------------------------------------------------------------------------------
// Begining of the "While" Operating System
//------------------------------------------------------------------------------
  while(ALWAYS) {                       // Can the Operating system run

     P2OUT |= IR_LED;

    P6_LCheck();
    P6_RCheck();

    LCD_update_ADC();

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
    case SWITCH_1:                       //straight
        Wheels_Process();
        //LINE_CHECK();
        //ADC_Switch = THUMB;
        break;
    case SWITCH_2:                       //circle
        //ADC_Switch = SENSOR;
        //LINE_CHECK();
        RIGHT_FORWARD_SPEED = WHEEL_OFF;
        LEFT_FORWARD_SPEED = WHEEL_OFF;
        RIGHT_REVERSE_SPEED = WHEEL_OFF;
        LEFT_REVERSE_SPEED = WHEEL_OFF;
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
