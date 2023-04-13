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

int ReadJTAG(){
    
}


int main(void)
{
    volatile Timer *const timer = (Timer *)MPCORE_PRIV_TIMER;
    volatile int *LED_ptr = (int *)LED_BASE;
    volatile jtag_uart* const uart_ptr = ( jtag_uart* )0xFF201000;
    int read_uart;


    volatile int interval = 2300000;
    timer->load = interval;
    int counter;
    int stats;
    int defaultTime = 300;
    int time = defaultTime;

    *(LED_ptr) |= 0x1;

    while (1)
    {
        counter = timer->count;
        stats = timer->status;
        
        int action = uart_ptr->data;

        if (action == 'v')
        {
            switch (action)
            {
            // Start
            case 's':
                timer->control = 3;
                break;

            // Pause
            case 'p':
                timer->control = 2;
                break;

            // Change Time
            case 'n':
                *(LED_ptr) |= 0x1;
                // code for changing time based on the JTAG UART Input
                break;

            // Clear
            case 'c':
                timer->count = interval;
                time = defaultTime;
                timer->status = 1;
                *(LED_ptr) |= 0x1;
                break;

            default:
                break;
            }
            DisplayHex(time);
        }

        if (time == 0)
        {
            *(LED_ptr) &= ~0x1;
            timer->status = 0;
            timer->control = 2;
        }
        if (stats == 1)
        {
            time--;
            timer->status = 1;
        }
    }
}