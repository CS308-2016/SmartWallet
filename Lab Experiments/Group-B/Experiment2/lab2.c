#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
void setup(void) {
	SysCtlClockSet(
	SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

void ledPinConfig(void) {
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
	GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

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

int pressed, pressed2, ui8LED;
int confirmed = 0;
int confirmed2 = 0;

int main(void) {
		setup();
		ledPinConfig();
		switchPinConfig();

	uint32_t ui32Period;
	SysCtlClockSet(
	SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
	GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // configuring timer to be periodic
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // setting period of timer
	ui32Period = (SysCtlClockGet() / 100);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    // enabling timer interrupts
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	pressed = 0;
	ui8LED = 2;
    // start red light
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, ui8LED);
	TimerEnable(TIMER0_BASE, TIMER_A);
	while (1) {
	}
}

// implementing deboucing via the state machine given in the question
// detecting key presses of switch 2
unsigned char detectKeyPress(void) {
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0) {
		if (pressed < 5)
			pressed++;
	} else {
		pressed = 0;
		int flag = 0;
		if(confirmed) flag = 1; // values being updated as per state machine
		confirmed = 0;
		return flag;
	}
	if (pressed == 2) {
		confirmed = 1;
		//return 1;
	}
	return 0;
}

// detecting key presses of switch 1
unsigned char detectKeyPress2(void) {
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0) {
		if (pressed2 < 5)
			pressed2++;
	} else {
		pressed2 = 0;
		int flag = 0;
		if(confirmed2) flag = 1; // values being updated as per state machine
		confirmed2 = 0;
		return flag;
	}
	if (pressed2 == 2) {
		confirmed2 = 1;
	}
	return 0;
}

int val = 0;

// the interrupt handler thats registered to be called on timer interrupts
void Timer0IntHandler(void) {
// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	unsigned char flag = detectKeyPress();
	unsigned char flag2 = detectKeyPress2();
	if (flag) {
		if (ui8LED == 8) {
			ui8LED = 2;
		} else {
			ui8LED = ui8LED * 2;
		}
        // light the next color in bulb
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, ui8LED);
	}
	if(flag2) {
		val++;
	}
}
