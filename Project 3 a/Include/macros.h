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

//clocks.c defines
#define MCLK_FREQ_MHZ           (8) // MCLK = 8MHz
#define CLEAR_REGISTER     (0X0000)

//global variables
extern char display_line[4][11];
extern char *display[4];
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;

#endif /* INCLUDE_MACROS_H_ */
