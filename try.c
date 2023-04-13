#include <stdint.h>

// Address mappings for DE1-SoC peripherals
#define LED_BASE        0xFF200000 // LED base address
#define HEX5_HEX4_BASE  0xFF200030 // Upper 2 digits of 7-segment display base address
#define HEX3_HEX0_BASE  0xFF200020 // Lower 4 digits of 7-segment display base address
#define JTAG_UART_BASE  0xFF201000 // JTAG UART base address

volatile int *LED_ptr = (int *)LED_BASE;
volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
volatile int *JTAG_UART_ptr = (int *)JTAG_UART_BASE;

void delay(int d) {
    volatile int delay_count = d;
    while (delay_count > 0)
        delay_count--;
}

// Function to convert hex digit to 7-segment display code
int hex_to_7segment(int hex) {
    int display_code;
    switch (hex) {
        case 0x0: display_code = 0x3F; break; // 0
        case 0x1: display_code = 0x06; break; // 1
        case 0x2: display_code = 0x5B; break; // 2
        case 0x3: display_code = 0x4F; break; // 3
        case 0x4: display_code = 0x66; break; // 4
        case 0x5: display_code = 0x6D; break; // 5
        case 0x6: display_code = 0x7D; break; // 6
        case 0x7: display_code = 0x07; break; // 7
        case 0x8: display_code = 0x7F; break; // 8
        case 0x9: display_code = 0x6F; break; // 9
        default:  display_code = 0x00; break; // Blank
    }
    return display_code;
}

int main() {
    int user_input;
    int timer = 0;
    int minutes, seconds, milliseconds;
    int hex5_hex4_mask, hex3_hex0_mask;
    int countdown_flag = 0; // Flag to indicate whether countdown is in progress

    while (1) {
        // Read user input from JTAG UART
        user_input = (*JTAG_UART_ptr) & 0xFF; // Mask to only get the lower 8 bits

        // Perform action based on user input
        switch (user_input) {
            case '1': // Turn on LED
                *LED_ptr = 0x01;
                break;
            case '2': // Turn off LED
                *LED_ptr = 0x00;
                break;
            case '3': // Add 10 seconds to timer and start countdown
                if (!countdown_flag) {
                    timer += 10000; // Add 10 seconds in milliseconds
                    countdown_flag = 1; // Set countdown flag
                }
                break;
            default:
                // Do nothing for invalid input
                break;
        }

        // Update minutes, seconds, and milliseconds from timer value
    minutes = timer / 60000;
    seconds = (timer % 60000) / 1000;
    milliseconds = (timer % 1000);

    // Convert minutes, seconds, and milliseconds to 7-segment display codes
    int hex5_hex4 = (minutes / 10) << 8 | (minutes % 10);
    int hex3_hex0 = (seconds / 10) << 24 | (seconds % 10) << 16 | (milliseconds / 100) << 8 | (milliseconds % 100) / 10;

    // Mask to extract individual digits for 7-segment display
    hex5_hex4_mask = (0xFF << 8) | 0xFF;
    hex3_hex0_mask = (0xFF << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF;

    // Display the digits on the 7-segment display
    *HEX5_HEX4_ptr = hex_to_7segment((hex5_hex4 & hex5_hex4_mask) >> 8) << 8 |
                     hex_to_7segment(hex5_hex4 & 0xFF);
    *HEX3_HEX0_ptr = hex_to_7segment((hex3_hex0 & hex3_hex0_mask) >> 24) << 24 |
                     hex_to_7segment((hex3_hex0 & (0xFF << 16)) >> 16) << 16 |
                     hex_to_7segment((hex3_hex0 & (0xFF << 8)) >> 8) << 8 |
                     hex_to_7segment(hex3_hex0 & 0xFF);

    // If countdown is in progress, decrement the timer
    if (countdown_flag) {
        timer -= 10; // Decrement timer by 10 milliseconds
        if (timer <= 0) {
            *LED_ptr = 0x00; // Turn off LED
            countdown_flag = 0; // Reset countdown flag
        }
    }

    delay(10); // Delay for 10 milliseconds
}

return 0;
}