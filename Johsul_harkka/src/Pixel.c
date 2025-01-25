/*
* Pixel.c
*
*  Created on: -----
*      Author: -----
*/

#include "Pixel.h"

//define control and channel signals
//Table for pixel dots.
//               dots[X][Y][COLOR]
volatile uint8_t dots[8][8][3] = {0};

// Here the setup operations for the LED matrix will be performed
void setup() {
    //1. Initialize control and chanel signals to 0
	CONTROL_SIGNAL = 0;
	CHANNEL_SIGNAL = 0;

	//reseting screen at start is a MUST to operation (Set RST-pin to 1).
	CONTROL_SIGNAL |= (1 << 0); // bit 0 to 1
	usleep(500); // wait 500myS
	CONTROL_SIGNAL &= ~(1 << 0); //clear bit 0 
	usleep(500); // wait 500myS
	CONTROL_SIGNAL |= (1 << 0); // bit 0 to 1

	//Set sda bit to 1
	CHANNEL_SIGNAL |= (1 << 4);
	
	//Write code that sets 6-bit values in register of DM163 chip. Recommended that every bit in that register is set to 1. 6-bits and 24 "bytes", so some kind of loop structure could be nice.
	//24*6 bits needs to be transmitted
	 uint8_t gamma_vector[3] = {63,63,63}; // Gamma correction values (all bits set to 1)

    for (int i = 0; i < 8; i++) { // Loop over 8 rows
        for (int color = 0; color < 3; color++) { // Loop over RGB colors
            uint8_t value = gamma_vector[color];
            for (int bit = 0; bit < 6; bit++) { // Loop over 6 bits
                if (value & 0x20) { // Check highest bit
                    CONTROL_SIGNAL |= (1 << 4); // SDA high
                } else {
                    CONTROL_SIGNAL &= ~(1 << 4); // SDA low
                }

                CONTROL_SIGNAL &= ~(1 << 3); // CLK low
                CONTROL_SIGNAL |= (1 << 3);  // CLK high
                value <<= 1; // Shift to the next bit
            }
        }
    }

    // Final step: Set SB-bit to 1 to enable transmission to the 8-bit register.
    CONTROL_SIGNAL |= (1 << 2); // Set bit 2 (SB) to 1

}

// Change value of one pixel at LED matrix. This function is only used for changing values of the dots array.
void SetPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    dots[x][7 - y][0] = b; // Blue
    dots[x][7 - y][1] = g; // Green
    dots[x][7 - y][2] = r; // Red
}



// new data to the LED matrix. This function is supposed to send 24-bytes.
void run(uint8_t x) {
    CONTROL_SIGNAL &= ~(1 << 1);  // latch off
    // rows, color, bits
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t color = 0; color < 3; color++) {
            for (uint8_t bit_pos = 0; bit_pos < 8; bit_pos++) {
                if (dots[x][y][color] & (0x80 >> bit_pos)) {
                    CONTROL_SIGNAL |= (1 << 4);  // SDA on
                } else {
                    CONTROL_SIGNAL &= ~(1 << 4);  // SDA of
                }

                
                CONTROL_SIGNAL &= ~(1 << 3);  // CLK down
                CONTROL_SIGNAL |= (1 << 3);   // CLK up
            }
        }
    }

    latch();  
    CONTROL_SIGNAL &= ~(1 << 3);  // CLK down
}

// Latch signal. See colorsshield.pdf or DM163.pdf in the project folder on how latching works.
void latch() {
    CONTROL_SIGNAL |= (1 << 1);
    CONTROL_SIGNAL &= ~(1 << 1);
}



// Set one line (channel) as active, one at a time.
void open_line(uint8_t channel) {
    // Clear all channels first to ensure only the selected channel is active
    CHANNEL_SIGNAL = 0;
    // Use switch-case to activate only the specified channel
    switch (channel) {
        case 0:
            CHANNEL_SIGNAL |= (1 << 0); // Activate channel 0
            break;
        case 1:
            CHANNEL_SIGNAL |= (1 << 1); // Activate channel 1
            break;
        case 2:
            CHANNEL_SIGNAL |= (1 << 2); // Activate channel 2
            break;
        case 3:
            CHANNEL_SIGNAL |= (1 << 3); // Activate channel 3
            break;
        case 4:
            CHANNEL_SIGNAL |= (1 << 4); // Activate channel 4
            break;
        case 5:
            CHANNEL_SIGNAL |= (1 << 5); // Activate channel 5
            break;
        case 6:
            CHANNEL_SIGNAL |= (1 << 6); // Activate channel 6
            break;
        case 7:
            CHANNEL_SIGNAL |= (1 << 7); // Activate channel 7
            break;
        default:
            // default case to clear all channels if an invalid channel number is provided
            CHANNEL_SIGNAL = 0;
            break;
    }
}
