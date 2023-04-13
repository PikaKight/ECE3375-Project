#include "address_map_arm.h"

#include <stdio.h>
#include <string.h>


#define JTAG_UART_CONTROL_WRITE_MASK 0x00000002;
#define JTAG_UART_CONTROL_READ_MASK 0x00000001;

// A9 Private Timer
typedef struct Timer
{
    int load;
    int count;
    int control;
    int status;
} Timer;

typedef struct _jtag_uart
{
  int data;
  int control;
} jtag_uart;

void DisplayHex(int value)
{
    int hundredth = value;
    int tenth = value / 10;
    int sec = value / 100;
    int tsec = value / 1000;
    int min = value / 6000;
    int tmin = value / 60000;

    volatile int hex_code[10] = {0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x67};
    volatile int *HEX_ptr = (int *)HEX3_HEX0_BASE;  // Gets the Lower 4 Sigment Address as Hex
    volatile int *HEX_ptr2 = (int *)HEX5_HEX4_BASE; // Gets the other part of the Lower 4 Sigment Address as Hex

    // time passed hundredth
    int dHundredth = hex_code[hundredth - (tenth * 10)];

    // time passed tenth
    int dTenth = hex_code[tenth - (sec * 10)] << 8;

    // time passed secs
    int dSec = hex_code[sec - (tsec * 10)] << 16;

    // time passed Ten Sec
    int dTenSec = hex_code[tsec - (min * 6)] << 24;

    // time passed Min
    int dMin = hex_code[min - (tmin * 10)];

    // time passed Ten Min
    int dTenMin = hex_code[tmin] << 8;

    int segDis1 = dHundredth + dTenth + dSec + dTenSec;
    int segDis2 = dMin + dTenMin;

    *HEX_ptr = segDis1;
    *HEX_ptr2 = segDis2;
}

int main(void)
{
    volatile Timer *const timer = (Timer *)MPCORE_PRIV_TIMER;   
    volatile int *LED_ptr = (int *)LED_BASE;
    volatile jtag_uart* const uart_ptr = ( jtag_uart* )0xFF201000;
    
    char* instrcut = "Controls:\n"
                       "\t0: Turns off the Light and Timer\n"
                       "\t1: Turns on the Light and Timer\n"
                       "\tIf the Light is on, the following controls will work\n"
                       "\ts: Start Timer\n"
                       "\tc: Resets Timer\n"
                       "\tp: Pause Timer\n"
                       "\tEnter Your Command:";
    
    

    for ( int i = 0; instrcut[i] != '\0'; i++ ){
        // write to JTAG UART
        uart_ptr->data = instrcut[i];
	}


    volatile int interval = 2300000;
    timer->load = interval;
    int counter;
    int stats;
    int defaultTime = 0;
    int time = defaultTime;
	int lightStat = 0;
	int timerActive = 0;
    int timeUpdated = 0; // Added flag to keep track of time update
    int prevTime = 0;    // Added variable to keep track of previous time

    *(LED_ptr) &= ~0x1;
    
    while (1)
    {
        counter = timer->count;
        stats = timer->status | 0;
        int action;
		action = uart_ptr->data;
			
		if (time == 0 && timerActive == 1)
        {
            *(LED_ptr) &= ~0x1;
			lightStat = 0;
			timerActive = 0;
        }
		
		if (lightStat == 1){ 
			switch (action)
			{
				// Start
				case 's':
					timer->control = 3;
					timerActive = 1;
					break;

				// Pause
				case 'p':
					timer->control = 2;
					break;

				// Clear
				case 'c':
					timer->count = interval;
					time = defaultTime;
					timer->status = 1;
					timerActive = 0;
					break;

                
				case '3':
                    if (!timeUpdated) // Check if time has been updated already
                    {
                        if (!timerActive) // Check if timer is paused
                        {
                            prevTime = time; // Save previous time
                        }
                        time += 100;
                        timerActive = 0; // Pause the timer
                        timeUpdated = 1; // Set the flag to indicate time update
                    }
                    break;
				
				// Change Time + 60 seconds
                case '5':
                    if (!timeUpdated) // Check if time has been updated already
                    {
                        if (!timerActive) // Check if timer is paused
                        {
                            prevTime = time; // Save previous time
                        }
                        time += 6000;
                        timerActive = 0; // Pause the timer
                        timeUpdated = 1; // Set the flag to indicate time update
                    }
                    break;

                default:
                    timeUpdated = 0; // Reset the flag if any other input is entered
                    break;
			}
		}
		
		switch(action){
			case '0':
				*(LED_ptr) &= ~0x1;
				lightStat = 0;
                time = defaultTime;
				break;
			case '1':
				*(LED_ptr) |= 0x1;
				lightStat = 1;
				break;
			
			default:
				break;
		}
		
        DisplayHex(time);
        
        if (stats == 1 && timerActive == 1)
        {
            time--;
            timer->status = 1;
        }
	}
}