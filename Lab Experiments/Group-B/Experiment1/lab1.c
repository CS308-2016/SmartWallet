#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include <stdbool.h>
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

void setup(void) {
	SysCtlClockSet(
	SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

 * Function Name: ledPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

 * Example Call: ledPinConfig();

 */

void ledPinConfig(void) {
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
	GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */
void switchPinConfig(void) {
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F = 0x4C4F434BU;
	CR_F = GPIO_PIN_0 | GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0,
	GPIO_STRENGTH_12MA,
	GPIO_PIN_TYPE_STD_WPU);

}
uint32_t sw2Status = 0;
int main(void) {
	uint8_t ui8LED = 2; // setting to initially turn on red light
	setup();
	ledPinConfig();
	switchPinConfig();
	uint8_t press = 0;
	uint8_t press2 = 0;
	while (1) {
		press = 1;
		press2 = 0;
        // till switch 1 is being pressed
		while (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00) {
			press2 = 1;
			if (press == 1) {
				//printf("apple mango");
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, ui8LED);
				// Delay for a bit
				press = 0;
			}
			uint8_t press3 = 0;
            // checking whether switch 2 is pressed
			while (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00) {
				press3 = 1;
			}
            // incrementing value of sw2Status in case switch 2 is pressed
			if (press3) {
				press3 = 0;
				sw2Status++;
			}
		}
        // clearing the color in bulb by setting every pin to 0
        // necessary because problem stmt asks to switch off bulb when switch is left being pressed
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);

        
        // in order to switch color on every press of switch sw2
		if (press2) {
			press2 = 0;
			if (ui8LED == 8) {
				ui8LED = 2;
			} else {
				ui8LED = ui8LED * 2;
			}
		}

		uint8_t press3 = 0;
        // checking whether switch 2 is pressed
		while (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00) {
			press3 = 1;
		}
        // incrementing value of sw2Status in case switch 2 is pressed
		if (press3) {
			press3 = 0;
			sw2Status++;
		}
	}
}