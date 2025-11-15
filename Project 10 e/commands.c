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
command_task task_list[MAX_TASKS]; //list of tasks

/*
 * Init_Task_List(void)
 * Arguments: void
 * initialize all tasks to Null
 */
void Init_Task_List(void) {
    //Initialize all tasks entries to empty state
    unsigned int i;

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
/*
 * Run_RTC_Scheduler(void)
 * Arguments: void
 * WRONG NAME
 * This is not an RTC scheduler
 * will check if first task can be ran
 * will remove first task once finished
 * will stop car if not supposed to move
 */
void Run_RTC_Scheduler(void) {
    //run for first in task
    if (task_list[0].ready == TRUE) {
        task_list[0].enabled = TRUE;
    } //if the first task is ready, enable
    if ((task_list[0].task != NULL_CHAR) && (task_list[0].enabled == TRUE) && (task_list[0].ready == TRUE)) { //if task is ready, enabled, and NOT a NULL_CHAR
        TB1CCTL2 |= CCIE;     //start timer
        run_task(); //run task
    }
    if (task_list[0].task != NULL_CHAR && task_list[0].period == 0) { //if first task is not NULL and task period is finished, remove task
        TB1CCTL2 &= ~CCIE;
        remove_task(0);
    }
    if (priority >= MAX_TASKS) { //if priority exceeds max tasks, reset to final task
        priority = MAX_TASKS-1;
    }
    if ((task_list[0].task == NULL_CHAR) && (Switch_State != BLACK)) { //if current task is NULL and car is NOT on Line Following, Stop Wheels
        RIGHT_FORWARD_SPEED = WHEEL_OFF;
        LEFT_FORWARD_SPEED = WHEEL_OFF;
        RIGHT_REVERSE_SPEED = WHEEL_OFF;
        LEFT_REVERSE_SPEED = WHEEL_OFF;
    }
}

/*
 * run_task(void)
 * Arguments:void
 * function runs inside Run_RTC_Scheduler
 * using the task, will run related function
 * if function is not coded for, remove task from list
 */
void run_task(void) {
    switch(task_list[0].task) { //switch case with the task
    case 'F': //if task is 'F', run forward
        run_forward(task_list[0].period);
        break;
    case 'B': //if task is 'B', run forward
        run_backward(task_list[0].period);
        break;
    case 'L': //if task is 'L', run forward
        run_left(task_list[0].period);
        break;
    case 'R': //if task is 'R', run forward
        run_right(task_list[0].period);
        break;
    case '1': //if task is '1', run forward
        arrived_1(task_list[0].period);
        break;
    case '2': //if task is '2', run forward
        arrived_2(task_list[0].period);
        break;
    case '3': //if task is '3', run forward
        arrived_3(task_list[0].period);
        break;
    case '4': //if task is '4', run forward
        arrived_4(task_list[0].period);
        break;
    case '5': //if task is '5', run forward
        arrived_5(task_list[0].period);
        break;
    case '6': //if task is '6', run forward
        arrived_6(task_list[0].period);
        break;
    case '7': //if task is '7', run forward
        arrived_7(task_list[0].period);
        break;
    case '8': //if task is '8', run forward
        arrived_8(task_list[0].period);
        break;
    case '0': //if task is '0', run forward
        Black_Line_Follow(task_list[0].period);
        break;
    case 'A': //if task is 'A', run arrived white
        arrived_white(task_list[0].period);
        break;
    case 'I': //if task is 'I', run arrived black
        arrived_black(task_list[0].period);
        break;
    case 'S': //if task is 'F', run forward
        Stop_All(task_list[0].period);
        break;
    case '9': //if task is '9', run Black Stop
        Black_Stop(task_list[0].period);
        break;
    default:  //if task is unknown, remove task
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
