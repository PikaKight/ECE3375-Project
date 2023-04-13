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

#define LED_BASE 0xFF200000

// A9 Private Timer
typedef struct Timer
{
    int load;
    int count;
    int control;
    int status;
} Timer;

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

int ReadSwitch(void)
{
    volatile int *SW_ptr = (int *)SW_BASE;
    int swVal = *SW_ptr;
    return swVal;
}

int ReadBtn(void)
{
    volatile int *BTN_ptr = (int *)KEY_BASE;
    int btnVal = *BTN_ptr;
    return btnVal;
}

void jtag_uart_write(int data){

}

int jtag_uart_read(){
    
}

int main(void)
{
    volatile Timer *const timer = (Timer *)MPCORE_PRIV_TIMER;
    volatile int *LED_ptr = (int *)LED_BASE;
    volatile int *JTAG_Dptr = (int *)JTAG_UART_BASE;
    volatile int *JTAG_Cptr = (int *)(JTAG_UART_BASE + 4);

    volatile int interval = 2300000;
    timer->load = interval;
    int counter;
    int stats;
    int defaultTime = 300;
    int time = defaultTime;

    *(LED_ptr) |= 0x1;

    while (1)
    {
        int swVal = ReadSwitch();
        counter = timer->count;
        stats = timer->status;

        if (swVal == 0)
        {
            int action = ReadBtn();

            switch (action)
            {
            // Start
            case 1:
                timer->control = 3;
                break;

            // Stop
            case 2:
                timer->control = 2;
                break;

            // adds time to timer
            case 4:
                *(LED_ptr) |= 0x1;
                if (time < 999999)
                {
                    time += 10;
                }
                break;

            // Clear
            case 8:
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