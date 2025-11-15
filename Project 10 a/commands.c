/*
 * commands.c
 *
 *  Created on: Apr 13, 2025
 *      Author: chait
 */
#include  "msp430.h"
#include  <string.h>
#include  "include\functions.h"
#include  "include\LCD.h"
#include  "include\ports.h"
#include "include\macros.h"


unsigned int priority = 0; //position in list of things to do
/*typedef struct {
    //unsigned char name; //name of function for wheels
    unsigned int period; //time for event
    unsigned int delay; //time after event
    unsigned int ready; //binary: 1 = run now
    unsigned int enabled; // active task
    unsigned char task;
    //void (* task)(int num); //address of function
} command_task;*/

//global list
command_task task_list[MAX_TASKS];

void Init_Task_List(void) {
    //Initialize all tasks entries to empty state
    int i;

    for (i = 0; i < MAX_TASKS; i++) {
        task_list[i].period = 0;
        task_list[i].delay = 0;
        task_list[i].ready = 0;
        task_list[i].enabled = 0;
        task_list[i].task = NULL_CHAR;
        //task_list.task = NULL;
    }
}
//rtc scheduler is for synchronous tasks, no can do?
void Run_RTC_Scheduler(void) {
    //run for first in task
    if (task_list[0].ready == TRUE) {
        task_list[0].enabled = TRUE;
    } //if the first task is ready, enable
    if ((task_list[0].task != NULL_CHAR) && (task_list[0].enabled == TRUE) && (task_list[0].ready == TRUE)) {
        TB1CCTL2 |= CCIE;     //start timer
        run_task();
    }
    if (task_list[0].task != NULL_CHAR && task_list[0].period == 0) {
        TB1CCTL2 &= ~CCIE;
        remove_task(0);
    }
    if (priority >= MAX_TASKS) {
        priority = MAX_TASKS-1;
    }
}

void run_task(void) {
    switch(task_list[0].task) {
    case 'F':
        run_forward(task_list[0].period);
        break;
    case 'B':
        run_backward(task_list[0].period);
        break;
    case 'L':
        run_left(task_list[0].period);
        break;
    case 'R':
        run_right(task_list[0].period);
        break;
    case '1':
        arrived_1(task_list[0].period);
        break;
    case '2':
        arrived_2(task_list[0].period);
        break;
    case '3':
        arrived_3(task_list[0].period);
        break;
    case '4':
        arrived_4(task_list[0].period);
        break;
    case '5':
        arrived_5(task_list[0].period);
        break;
    case '6':
        arrived_6(task_list[0].period);
        break;
    case '7':
        arrived_7(task_list[0].period);
        break;
    case '8':
        arrived_8(task_list[0].period);
        break;
    case '0':
        //Black_Line_Follow(task_list[0].period);
        break;
    case 'S':
        //Stop_All(task_list[0].period);
        break;
    default:
        remove_task(0);
        break;
    }
}


//add task to list
//redone, don't need priority, do need time. will not be given task, so need another function to do so?
int add_task(unsigned char task, unsigned int time) {
    if (task_list[priority].task == NULL_CHAR) {
        //if the position is not yet full
        task_list[priority].task = task;
        task_list[priority].ready = 0;
        task_list[priority].delay = DELAY_TIME;
        task_list[priority].period = time;
        task_list[priority].enabled = 0;
        //update priority
        priority ++;
        if (priority >= MAX_TASKS) {
            priority = MAX_TASKS-1;
        }
        display_line[3][0] = 'G';
        return 1; //did manage to add task
    }
    else {
        display_line[3][0] = 'P';
    }
    return 0; //did not add task, probably won't be checking this?
}

//given function removes task by name, rather remove by position, can use for moving tasks
void remove_task(unsigned int position) {
    unsigned int i = 0;
    if (position < MAX_TASKS) {
        task_list[position].task = NULL_CHAR;
        task_list[position].delay = 0;
        task_list[position].period = 0;
        task_list[position].ready = 0;
        task_list[position].enabled = 0;
    }
    for (i = position; i < MAX_TASKS-1; i++) {
        task_list[i].task = task_list[i+1].task;
        task_list[i].delay = task_list[i+1].delay;
        task_list[i].period = task_list[i+1].period;
        task_list[i].ready = task_list[i+1].ready;
        task_list[i].enabled = task_list[i+1].enabled;
    }
    if (priority > 0) {
        priority --;
    }
}
