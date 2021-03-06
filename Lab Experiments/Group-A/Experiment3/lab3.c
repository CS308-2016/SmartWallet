/*
 *
 * Author List: Shivm Garg, Aditya Nambiar
 * Filename: lab3.c
 * Functions: setup(), pwmsetup(), ledPinConfig, setTimer, detectKey2Press, detectKey1Press, decreaseCounter, increaseCounter, changeColor, TimerInt0Handler
 * Global Variables: ui32Load, curr_counter, change_counter, low_limit, s1_state, s2_state, s1_counter, s2_counter, long_press_limit, auto_state, mode, ui8Adjust_red, ui8Adjust_green, ui8Adjust_blue
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

volatile uint32_t ui32Load;
int curr_counter = 0;
int change_counter = 5;
int low_limit = 1;
int high_limit = 100;
int s1_state = 0;
int s2_state = 0;
int s1_counter = 0;
int s2_counter = 0;
int long_press_limit = 300;
int auto_state = 0;
int mode = 0;
int ui8Adjust_red = 100;
int ui8Adjust_blue = 100;
int ui8Adjust_green = 100;




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

* Function Name: pwmsetup()

* Input: none

* Output: none

* Description: Enable PWM peropheral and configure corresponding GPIO 

* Example Call: pwmsetup();

*/
void pwmsetup(void){
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
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
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*
*
* Function Name: seTimer
* Input: None
* Output: None
* Logic:Enables peripheral for timer0 and also enables interrupt.
* Example Call: setTimer()
*
*/
void setTimer(void)
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = SysCtlClockGet() / 100;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}


/*
*
* Function Name: detectKey2Press()
* Input: None
* Output: integer
* Logic:Enables detects whether key2 is short pressed, long pressed or not pressed
* Example Call: detectKey2Press()
*
*/

int detectKey2Press(){
	unsigned char flag = 0;
	int32_t not_pressed = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
	if(s2_state == 0){
		if(!not_pressed)
			s2_state = 1;
	}
	else if(s2_state == 1){
		if(!not_pressed){
			s2_state = 2;
			flag = 1;
			s2_counter++;
		}
		else{
			s2_state = 0;
		}
	}
	else if(s2_state == 2){
		if(not_pressed){
			s2_state = 0;
			s2_counter = 0;
		}
		else{
			s2_counter++;
			if(s2_counter>long_press_limit){
				s2_counter = long_press_limit;
				flag = 2;
			}
		}
	}
	return flag;
}

/*
*
* Function Name: detectKey1Press()
* Input: None
* Output: integer
* Logic:Enables detects whether key1 is short pressed, long pressed or not pressed
* Example Call: detectKey1Press()
*
*/

int detectKey1Press(){
	unsigned char flag = 0;
	int32_t not_pressed = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	if(s1_state == 0){
		if(!not_pressed)
			s1_state = 1;
	}
	else if(s1_state == 1){
		if(!not_pressed){
			s1_state = 2;
			flag = 1;
			s1_counter++;
		}
		else{
			s1_state = 0;
		}
	}
	else if(s1_state == 2){
		if(not_pressed){
			s1_state = 0;
			s1_counter = 0;
		}
		else{
			s1_counter++;
			if(s1_counter>long_press_limit){
				s1_counter = long_press_limit;
				flag = 2;
			}
		}
	}
	return flag;
}

/*
*
* Function Name: decrease_counter
* Input: None
* Output: None
* Logic:Decrease counter for intensity
* Example Call: decrease_counter()
*
*/
void decrease_counter(void){
	if(change_counter!=low_limit){
		if(change_counter>20) change_counter-=10;
		else change_counter--;
	}
	return;
}

/*
*
* Function Name: increase_counter
* Input: None
* Output: None
* Logic:Increase counter for intensity
* Example Call: increase_counter()
*
*/

void increase_counter(void){
	if(change_counter!=high_limit){
		if(change_counter>=20) change_counter+=10;
		else change_counter++;
	}
	return;
}
/*
*
* Function Name: changeColor
* Input: None
* Output: None
* Logic: Change color to next color in the spectrum
* Example Call: changeColore
*
*/
void changeColor(){
	auto_state = (auto_state+1)%6;
	if(auto_state==0){
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , false);
	}
	else if(auto_state==1){
		PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , true);
	}
	else if(auto_state==2){
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , false);
	}
	else if(auto_state==3){
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , true);
	}
	else if(auto_state==4){
		PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , false);
	}
	else if(auto_state==5){
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
	}
	return;
}

/*
* Function Name: Timer0IntHandler
* Input: None
* Output: None
* Logic: Interrupt handler for timer 0. Called every 10 ms. Checks whether switch is pressed according to the state diagram and performs the appropriate action.
* Example Call: Automatically called every 10ms
*
*/

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	int key1 = detectKey1Press();
	int key2 = detectKey2Press();
	if(mode==0){
		if(key1==1) decrease_counter();
		if(key2==1) increase_counter();

		curr_counter++;
		if(curr_counter>change_counter){
			curr_counter = 0;
			changeColor();
		}
		if(key2==2){
			if(key1==1) mode = 1;
			if(key1==2) mode = 6;
			if(key1 > 0){
				PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
				PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , true);
				PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , true);
			}
		}
	}
	else if(mode==1){
		if(key2==0) mode = 2;
		else if(key1==1) mode = 3;
		else if(key1==2) mode = 6;
	}
	else if(mode==2){
		if(key2==2){
			if(key1==1) mode = 1;
			if(key1==2) mode = 6;
		}
		else{
			if(key1==1){
				if(ui8Adjust_red>1) ui8Adjust_red--;
			}
			if(key2==1){
				if(ui8Adjust_red<255) ui8Adjust_red++;
			}
		}
	}
	else if(mode==3){
		if(key2==0) mode = 4;
		else if(key1==2) mode = 6;
	}
	else if(mode==4){
		if(key2==2){
			if(key1==1) mode = 1;
			if(key1==2) mode = 6;
		}
		else{
			if(key1==1){
				if(ui8Adjust_blue>1) ui8Adjust_blue--;
			}
			if(key2==1){
				if(ui8Adjust_blue<255) ui8Adjust_blue++;
			}
		}
	}
	else if(mode==5){
		if(key2==2){
			if(key1==1) mode = 1;
			if(key1==2) mode = 6;
		}
		else{
			if(key1==1){
				if(ui8Adjust_green>1) ui8Adjust_green--;
			}
			if(key2==1){
				if(ui8Adjust_green<255) ui8Adjust_green++;
			}
		}
	}
	else if(mode==6){
		if(key2==0) mode = 5;
		else if(key1==1) mode = 1;
		else if(key1==2) mode = 6;
	}
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust_red * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust_blue * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust_green * ui32Load / 1000);
}

int main(void)
{
	volatile uint32_t ui32PWMClock;
	volatile uint8_t ui8Adjust;
	ui8Adjust = 254;

	setup();
	pwmsetup();

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
	//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
	//PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , false);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , false);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	setTimer();

	while(1)
	{
	}

}
