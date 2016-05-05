/*
 *
 * Author List: Shivm Garg, Aditya Nambiar
 * Filename: lab2.c
 * Functions: setup(), ledPinConfig, switchPinConfig , Timer0IntHandler
 * Global Variables: sw1st, sw2st, flag1, flag2, sw2Status, ui8LED
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"





#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
int sw1st=0;
int sw2st=0;
int flag1=0;
int flag2=0;
int sw2Status=1;
uint8_t ui8LED = 2;

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


/*

* Function Name: ledPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

* Example Call: ledPinConfig();

*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*

* Function Name: switchPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

* Example Call: switchPinConfig();

*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}



/*
* Function Name: Timer0IntHandler
* Input: None
* Output: None
* Logic: Interrupt handler for timer 0. Called every 10 ms. Checks which key is pressed on keypad according to the state diagram and performs the appropriate action.
* Example Call: Automatically called every 10ms
*
*/
void Timer0IntHandler(void)
{
// Clear the timer interrupt
TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
// Read the current state of the GPIO pin and
// write back the opposite state

if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && sw1st==0)
{
	sw1st=1;
}
else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && sw1st==1)
{
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
	flag1=1;
	sw1st=2;

}
else if(sw1st==1)
{
	sw1st=0;
}
else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0x00 && sw1st==2)
{
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	if (ui8LED == 8)
		{
			ui8LED = 2;
		}
		else
		{
			ui8LED = ui8LED*2;
		}
	flag1=0;
	sw1st=0;
}



if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00 && sw2st==0)
{
	sw2st=1;
}
else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00 && sw2st==1)
{
	sw2Status++;
	flag2=1;
	sw2st=2;

}
else if(sw2st==1)
{
	sw2st=0;
}

else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0x00 && sw2st==2)
{
	flag2=0;
	sw2st=0;
}

}

int checkSw1()
{
	return flag1;
}
int checkSw2()
{
	return flag2;
}


int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	uint32_t ui32Period;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 1);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);



	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

while(1){
}

}
