/*
 * macros.h
 *
 *  Created on: Jan 30, 2025
 *      Author: chait
 */

#ifndef INCLUDE_MACROS_H_
#define INCLUDE_MACROS_H_

//main.c defines
#define ALWAYS                  (1)
#define RESET_STATE             (0)
#define RED_LED              (0x01) // RED LED 0
#define GRN_LED              (0x40) // GREEN LED 1
#define TEST_PROBE           (0x01) // 0 TEST PROBE
#define TRUE                 (0x01) //

// STATES ======================================================================
#define NONE ('N')
//#define STRAIGHT ('L')
#define SWITCH_1 ('L')
//#define CIRCLE ('C')
#define SWITCH_2 ('C')
#define WAIT ('W')
#define START ('S')
#define RUN ('R')
#define END ('E')
#define ON ('N')
#define OFF ('F')
#define WHEEL_COUNT_TIME (100)
#define RIGHT_COUNT_TIME (100)
#define LEFT_COUNT_TIME (100)
#define TRAVEL_DISTANCE (2)
#define WAITING2START (300)
#define DEFINED_PERIOD (50)
#define TOO_FAST (30)

//clocks.c defines
#define MCLK_FREQ_MHZ           (8) // MCLK = 8MHz
#define CLEAR_REGISTER     (0X0000)

//switches.c defines
#define PRESSED         (0x01)        //switch pressed
#define RELEASED        (0x00)        //switch released
#define OKAY            (0x01)        //okay to look at switch
#define NOT_OKAY        (0x00)        //not okay to look at switch
#define DEBOUNCE_TIME   (100)         //EDITABLE debounce time
#define DEBOUNCE_RESTART (0x00)       //set debounce to zero


//global variables
extern char display_line[4][11];
extern char *display[4];
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;

extern unsigned int Last_Time_Sequence;     //A variable to identify Time_Sequence has changed
extern unsigned int cycle_time;             //is a new time base used to control making shapes
extern unsigned int time_change;            //is an identifier that a change has occurred
extern unsigned int Switch_State;           //identifies the state in switch state machine


#endif /* INCLUDE_MACROS_H_ */
