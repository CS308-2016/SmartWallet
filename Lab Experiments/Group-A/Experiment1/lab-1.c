/*
 *
 * Author List: Shivm Garg, Aditya Nambiar
 * Filename: lab1.c
 * Functions: setup(), ledPinConfig, switchPinConfig 
 * Global Variables: sw2Status
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
// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
int sw2Status=1;

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals  

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
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

int main(void)
{
	uint8_t ui8LED = 2;
	setup();
	ledPinConfig();
	switchPinConfig();

	int pres=0;
	int pres2=0;

while(1){

	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && pres!=1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		pres=1;

	}
	//SysCtlDelay(2000000);
	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)!=0x00 && pres==1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		pres=0;
	}

	if(pres==0)
	{
	pres=2;
	if (ui8LED == 8)
	{
		ui8LED = 2;
	}
	else
	{
		ui8LED = ui8LED*2;
	}
	}



	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00 && pres2==0)
		{
			pres2=1;
		}
	SysCtlDelay(100000);
	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00 &&  pres2==1 )
	{
		sw2Status++;
		pres2=2;
	}
	else if(pres2==1)
	{
		pres2=0;
	}

		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)!=0x00 && pres2==2)
		{
			pres2=0;
		}

}

}
