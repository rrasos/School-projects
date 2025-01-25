/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// Main program for exercise

//****************************************************
//By default, every output used in this exercise is 0
//****************************************************
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"




//********************************************************************
//***************TRY TO READ COMMENTS*********************************
//********************************************************************

//***Hint: Use sleep(x)  or usleep(x) if you want some delays.****
//***To call assembler code found in blinker.S, call it using: blinker();***


//Comment this if you want to disable all interrupts
#define enable_interrupts


/***************************************************************************************
Name:Rasmus Sorila
Student number:

Name:Oskar Kainulainen
Student number:

Tick boxes that you have coded

Led-matrix driver		Game		    Assembler
	[x]					[x]					[x]

Brief description:

*****************************************************************************************/

volatile uint8_t current_channel = 0;

void AlienMovementHandler(void);
void LaserHandler(void);
extern void blinker(void);

uint8_t channel_line = 0;
uint8_t alien = 0;
uint8_t direction = 0;
uint8_t x = 4;
uint8_t laser_on = 0;
uint8_t laser_y = 2;
uint8_t laser_x = 4;
uint8_t misses = 7;
uint8_t points = 0;

void create_ship(uint8_t x);

int main()
{
	//**DO NOT REMOVE THIS****
	    init_platform();
	//************************


#ifdef	enable_interrupts
	    init_interrupts();
#endif

	    //setup screen
	    setup();

	    Xil_ExceptionEnable();

		//initialises ship in middle
		create_ship(4);


	    //Try to avoid writing any code in the main loop.
		while(1){
			blinker();

		}

		cleanup_platform();
		return 0;
}


//Timer interrupt handler for led matrix update. Frequency is 800 Hz
void TickHandler(void *CallBackRef){
	//Don't remove this
	uint32_t StatusEvent;

	//exceptions must be disabled when updating screen
	Xil_ExceptionDisable();

	//**OWN CODE STARTS */

	//close all chanels
	CHANNEL_SIGNAL = 0;
	run(current_channel);
	open_line(current_channel);
	current_channel = (current_channel + 1) % 8;
	//****END OF OWN CODE*****************

	//*********clear timer interrupt status. DO NOT REMOVE********
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	//*************************************************************
	//enable exceptions
	Xil_ExceptionEnable();
}


void TickHandler1(void *CallBackRef){

    // Don't remove this
    uint32_t StatusEvent;

    // Call modular functions for alien and laser handling
    AlienMovementHandler();
    LaserHandler();

    // Clear timer interrupt status. DO NOT REMOVE
    StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
    XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
}

void AlienMovementHandler(void) {

	//moves right
    if (direction == 0) {
        SetPixel(alien, 6, 55, 0, 200);
        SetPixel(alien - 1, 6, 0, 0, 0);

		
        if (alien == 7) {
            direction = 1; //switch direction
        } else {
            alien++;
        }
    } else if (direction == 2) {
        // Handle end-game or specific direction case
    } 
	
	//direction to the left
	else {
        if (alien == 0) {
            direction = 0;  //switch direction
        } else {
            alien--;
        }

        SetPixel(alien, 6, 55, 0, 200);
        SetPixel(alien + 1, 6, 0, 0, 0);
    }
}

// Function to handle laser behavior
void LaserHandler(void) {

	//allows only one laser at time
    if (laser_on == 1) {
        if (laser_y < 7) {
            SetPixel(laser_x, laser_y, 50, 0, 0);
        } else if (laser_y == 7) {

			//clear laser
            SetPixel(laser_x, laser_y - 1, 0, 0, 0);
        }


		//move up
        if (laser_y > 2) {
            SetPixel(laser_x, laser_y - 1, 0, 0, 0);
        }

        if (laser_y == 6 && alien == laser_x) {
            // Alien is hit
            SetPixel(laser_x, laser_y, 0, 0, 0);
            SetPixel(alien, 6, 0, 0, 0);
            SetPixel(alien - 1, 6, 0, 0, 0);
            laser_on = 0;
            laser_y = 2;
            alien = 0;
            SetPixel(points, 7, 0, 50, 0);
            points++;

            if (points == 5) {
                // Win condition
                for (int it = 0; it < 8; it++) {
                    for (int j = 0; j < 8; j++) {
                        SetPixel(it, j, 0, 50, 0); // Green color for win
                    }
                }
                direction = 2; // Stop movement
            }
        } else if (laser_y < 7) {
            laser_y++;
        } else {
            // Laser missed
            SetPixel(misses, 7, 50, 0, 0); // Red color for miss
            misses--;
            laser_on = 0;
            laser_y = 2;

            if (misses == 2) {
                // Lose condition
                for (int it = 0; it < 8; it++) {
                    for (int j = 0; j < 8; j++) {
                        SetPixel(it, j, 50, 0, 0); // Red color for loss
                    }
                }
                direction = 2; // Stop movement
            }
        }
    }
}

//Interrupt handler for switches and buttons.
//Reading Status will tell which button or switch was used
//Bank information is useless in this exercise
void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status) 
{
	//****Write code here ****
	//Hint: Status==0x01 ->btn0, Status==0x02->btn1, Status==0x04->btn2, Status==0x08-> btn3, Status==0x10->SW0, Status==0x20 -> SW1


    // Clear all pixels in the ship's possible area (y = 0 and y = 1)
    for (int i = 0; i < 8; i++) {
        SetPixel(i, 0, 0, 0, 0); // Clear bottom row
        SetPixel(i, 1, 0, 0, 0); // Clear middle-upper row
    }

    // Move the ship left
    if (Status == 0x08) { // btn3
        if (x > 1) { // Prevent moving out of bounds
            x--; // Update ship position
        }
    }

    // Move the ship right
    if (Status == 0x04) { // btn2
        if (x < 6) { // Prevent moving out of bounds
            x++; // Update ship position
        }
    }

    // Fire the laser
    if (Status == 0x01) { // btn0
        if (laser_on == 0) { // Only allow one laser at a time
            laser_on = 1;
            laser_x = x;   // Laser starts from the middle of the ship
            laser_y = 2;   // Just above the ship
        }
    }


	//when switch 1 is used game resets
	if (Status == 0x20){
		restartGame();
	}


    // Redraw the ship at its new position
    create_ship(x);
}


void create_ship(uint8_t x)
{
	SetPixel(x-1,0,128,0,128);
	SetPixel(x,0,128,0,128);
	SetPixel(x,1,128,0,128);
	SetPixel(x+1,0,128,0,128);
}

void restartGame()
{
	//clear screen
	for (int it = 0; it < 8; it++) {
        for (int j = 0; j < 8; j++) {
            SetPixel(it, j, 0, 0, 0); 
        }
    }

//reset all variables
channel_line = 0;
alien = 0;
direction = 0;
x = 4;
laser_on = 0;
laser_y = 2;
laser_x = 4;
misses = 7;
points = 0;
}