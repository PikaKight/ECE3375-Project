/* This files provides address values that exist in the system */

#define BOARD "DE10-Standard"

/* Memory */
#define DDR_BASE 0x00000000
#define DDR_END 0x3FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define FPGA_PIXEL_BUF_BASE 0xC8000000
#define FPGA_PIXEL_BUF_END 0xC803FFFF
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF

/* Cyclone V FPGA devices */
#define LED_BASE 0xFF200000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define TIMER_2_BASE 0xFF202020
#define AV_CONFIG_BASE 0xFF203000
#define RGB_RESAMPLER_BASE 0xFF203010
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO0_BASE 0xFF708000
#define HPS_GPIO1_BASE 0xFF709000
#define HPS_GPIO2_BASE 0xFF70A000
#define I2C0_BASE 0xFFC04000
#define I2C1_BASE 0xFFC05000
#define I2C2_BASE 0xFFC06000
#define I2C3_BASE 0xFFC07000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define HPS_RSTMGR 0xFFD05000
#define HPS_RSTMGR_PREMODRST 0xFFD05014
#define FPGA_BRIDGE 0xFFD0501C

#define PIN_MUX 0xFFD08400
#define CLK_MGR 0xFFD04000

#define SPIM0_BASE 0xFFF00000
#define SPIM0_SR 0xFFF00028
#define SPIM0_DR 0xFFF00060
/* ARM A9 MPCORE devices */
#define PERIPH_BASE 0xFFFEC000       // base address of peripheral devices
#define MPCORE_PRIV_TIMER 0xFFFEC600 // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF 0xFFFEC100 // PERIPH_BASE + 0x100
#define ICCICR 0x00                 // offset to CPU interface control reg
#define ICCPMR 0x04                 // offset to interrupt priority mask reg
#define ICCIAR 0x0C                 // offset to interrupt acknowledge reg
#define ICCEOIR 0x10                // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST 0xFFFED000 // PERIPH_BASE + 0x1000
#define ICDDCR 0x00                // offset to distributor control reg
#define ICDISER 0x100              // offset to interrupt set-enable regs
#define ICDICER 0x180              // offset to interrupt clear-enable regs
#define ICDIPTR 0x800              // offset to interrupt processor targets regs
#define ICDICFR 0xC00              // offset to interrupt configuration regs

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
                       
    // for ( int i = 0; instrcut[i] != '\0'; i++ ){
    //     // write to JTAG UART
    //     uart_ptr->data = instrcut[i];
	// }


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