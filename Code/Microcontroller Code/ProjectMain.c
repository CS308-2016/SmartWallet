/*
 *
 * Team Leader Name: Mihir Kulkarni
 * Author List: Shivam Garg, Mihir Kulkarni
 * Filename: ProjectMain.c
 * Functions:charRead(), initState(), controller(unsigned char input), get_cur_sec(), setupRTC(),
 * 			 send_time(), Timer0IntHandler(void), setupTimer(), main(void)
 * Global Variables: inpChar, D0, D1, D2, D3, delay, iteration, memory, digits[10], operations[4], memory, newNum, credit, oldOp, dig, nextInp
 * 				     current_seconds, base_time, sentVal, KeySt, lastKey, fppKey, c, cl_flag
 *
 */


#include "commonheader.h"			// header file containing all required header files
#include "lcd.h"					// LCD header file


#define U_LIMIT 9999999
#define L_LIMIT -9999999

volatile unsigned char inpChar;
volatile int D3, D2, D1, D0;
volatile int delay;
volatile int iteration;

volatile int memory;
char digits[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
char operations[4] = { 'A', 'B', 'C', 'D' };



/*
*
* Function Name: charRead
* Input: None
* Output: Character pressed on the keypad
* Logic: Send 1 on the columns of keypad one by one, aqnd check the values at rows to get which key is pressed
* Example Call: charRead()
*
*/

int charRead() {

	SysCtlDelay(delay);
	iteration = iteration + 1;

	//-------------Column 1-----------------
	GPIOPinWrite(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x08);


	SysCtlDelay(delay);

	// Return corresponding value from first column
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) != 0)
		return '1';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) != 0)
		return '4';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) != 0)
		return '7';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) != 0)
		return '*';

	//-------------Column 2-----------------
	GPIOPinWrite(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x04);

	//Debounce
	SysCtlDelay(delay);

	// Return corresponding value from second column
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) != 0)
		return '2';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) != 0)
		return '5';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) != 0)
		return '8';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) != 0)
		return '0';

	//-------------Column 3-----------------
	GPIOPinWrite(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02);

	//Debounce
	SysCtlDelay(delay);

	// Return corresponding value from third column
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) != 0)
		return '3';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) != 0)
		return '6';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) != 0)
		return '9';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) != 0)
		return '#';

	//-------------Column 4-----------------
	GPIOPinWrite(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x01);


	SysCtlDelay(delay);

	// Return corresponding value from fourth column
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) != 0)
		return 'A';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) != 0)
		return 'B';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) != 0)
		return 'C';
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) != 0)
		return 'D';

	//---------------
	return '!';

}



/*
*
* Function Name: digit
* Input: character from '0' to '9'
* Output: character converted to integer
* Logic:converts characters to corresponding digits
* Example Call: digit('8')
*
*/

int digit(char c) {
	int i;
	for (i = 0; i < 10; i++) {
		if (digits[i] == c)
			return i;
	}
	return -1;
}



/*
*
* Function Name: isOperation
* Input: character
* Output: True/False
* Logic: checks if the character corresponds to some operation or not
* Example Call: isOperation('A')
*
*/
bool isOperation(char c) {
	int i;
	for (i = 0; i < 4; i++) {
		if (operations[i] == c)
			return true;
	}
	return false;
}

int calculate(int mem, char op, int inp) {
	switch (op) {
	case 'A':
	{
		if(mem + inp > U_LIMIT || mem + inp < L_LIMIT || mem + inp < mem)
			return U_LIMIT+1;
		return (mem + inp);
	}

	case 'B':
	{
		if(mem - inp > U_LIMIT || mem - inp < L_LIMIT || mem - inp > mem)
			return U_LIMIT+1;
		return (mem - inp);
	}

	case 'C':
	{
		if(mem * inp > U_LIMIT || mem * inp < L_LIMIT || mem * inp < mem && mem > 0 || mem*inp>mem && mem<0)
			return U_LIMIT+1;
		return (mem * inp);
	}

	case 'D':
	{
		if(inp == 0)
			return U_LIMIT+1;
		return (mem / inp);
	}

	default:
		return inp;
	}

}




volatile int memory, newNum, credit;
volatile char oldOp;
volatile int dig;
volatile char nextInp;


/*
*
* Function Name: initStatr()
* Input: None
* Output: 0
* Logic: Initialize the state for keypad
* Example Call: initState()
*
*/
int initState() {
	memory = 0;
	newNum = 0;
	credit = 0;
	nextInp = oldOp = '!';
	return 0;
}


/*
*
* Function Name: controller
* Input: character
* Output: integer
* Logic: Takes appropriate action for the given character. Updates the keypad state. For example. if newNum is 13 and input character is '5', it will
* 	     change newNum to 135.
* Example Call: controller('5')
*
*/

int controller(unsigned char input) {
	dig = digit(input);
	if (dig!= -1) {
		newNum = newNum*10 + dig;
	} else if (isOperation(input)) {
		int tempm;
		tempm = calculate(memory, oldOp, newNum);
		if(tempm != U_LIMIT + 1)
		{
			memory=tempm;
			oldOp = input;
			newNum = 0;
		}
		else
		{
			newNum = 0;
			return -1;
		}
	} else if (input == '*') {
		initState();
	} else if (input == '#') {
		if(memory != 0)
		{
			int tempm;
			tempm = calculate(memory, oldOp, newNum);
			if(tempm != U_LIMIT+1)
			{
				memory=tempm;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			memory= newNum;
		}
		newNum=0;
	}
	return 1;
}




unsigned int current_seconds;
unsigned int base_time = 1459932300; //2:15 PM 6th April


/*
*
* Function Name: get_cur_sec
* Input: None
* Output: current time since epoch time in seconds
* Logic: Add the base time to RTC time and return.
* Example Call: get_cur_sec()
*
*/
unsigned int get_cur_sec()
{
	return base_time + HibernateRTCGet();
}

/*
*
* Function Name: setupRTC
* Input: None
* Output: None
* Logic: Enable peripheral for RTC and enable RTC
* Example Call: setupRTC()
*
*/


void setupRTC()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	HibernateEnableExpClk(SysCtlClockGet());
	HibernateRTCEnable();

}

/*
*
* Function Name: send_time
* Input: None
* Output: None
* Logic: Send current time in seconds via UART1 to bluetooth
* Example Call: send_time()
*
*/


void send_time()
{
	unsigned int tempNum2=get_cur_sec();
	char tempChar2[40];
	int tempCo2=0;
	while(tempNum2!=0)
	{
		tempChar2[tempCo2]=(tempNum2%10)+'0';
		tempNum2=tempNum2/10;
		tempCo2++;
	}
	while(tempCo2!=0)
	{
		tempCo2--;
		UARTCharPut(UART1_BASE, tempChar2[tempCo2]);
	}
}




int sentVal=0;



int KeySt = 0; //Start state
char lastKey = '!'; //Last key pressed
char fppKey = '!'; //Fully pressed previous key


char c;
int cl_flag=0;



/*
*
* Function Name: Timer0IntHandler
* Input: None
* Output: None
* Logic: Interrupt handler for timer 0. Called every 10 ms. Checks which key is pressed on keypad according to the state diagram and performs the appropriate action.
* Example Call: Automatically called every 10ms
*
*/
void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	inpChar = charRead();
	int flagT = 0;


	if(inpChar!='!')
	{
		if(KeySt==0 || lastKey!=inpChar)
		{
			KeySt = 1; //Partially presses state
			lastKey = inpChar;
		}
		else if(KeySt==1)
		{
			KeySt = 2;//Pressed state
			flagT = 1;
		}

	}
	else
	{

		KeySt = 0;
		lastKey = '!';
	}

	if(flagT == 1)
	{
		if(fppKey != '#')
		{

			int tval1 = controller(inpChar);
			if(tval1==-1)
			{
				initState();
				lcd_command(0x01); // clear screen command
				lcd_command(0x80); // Set cursor to second line starting
				lcd_string("Overflow!");
				lcd_command(0x01); // clear screen command
				lcd_command(0x80); // Set cursor to second line starting

			}
			else if (inpChar != '!' && inpChar!='#')
			{


				if(inpChar >= '0' && inpChar <= '9')
				{
					if(cl_flag==1)
					{
						lcd_command(0x01); // clear screen command
						lcd_command(0x80); // Set cursor to second line starting
						cl_flag=0;
					}
					if(newNum>U_LIMIT)
					{
						newNum/=10;
					}
					else
					{
					lcd_char(inpChar);
					}

				}
				else
				{
					cl_flag=1;

					lcd_command(0x01); // clear screen command
					lcd_command(0x80); // Set cursor to second line starting
					int tempNum=memory;
					char tempChar[30];
					int tempCo=0;
					if(tempNum<0)
					{
						tempNum=tempNum*(-1);
						lcd_char('-');
					}
					while(tempNum!=0)
					{
						tempChar[tempCo]=(tempNum%10)+'0';
						tempNum=tempNum/10;
						tempCo++;
					}
					while(tempCo!=0)
					{
						tempCo--;
						lcd_char(tempChar[tempCo]);

					}

				}

			}
			else if(inpChar=='#')
			{
				sentVal=sentVal+1;

				lcd_command(0x01); // clear screen command
				lcd_command(0x80); // Set cursor to second line starting
				int tempNum=memory;
				char tempChar[30];
				int tempCo=0;
				if(tempNum<0)
				{
					tempNum=tempNum*(-1);
					lcd_char('-');
				}
				while(tempNum!=0)
				{
					tempChar[tempCo]=(tempNum%10)+'0';
					tempNum=tempNum/10;
					tempCo++;
				}
				while(tempCo!=0)
				{
					tempCo--;
					lcd_char(tempChar[tempCo]);

				}

			}
		}
		else if(fppKey == '#')
		{


			if (inpChar == 'A') {

				credit = 1;
			}
			else if (inpChar == 'B') {

				credit = -1;
			} else {

				credit = 0;
			}
			sentVal = memory;
			if(sentVal<0)
			{
				sentVal=sentVal*(-1);
				credit=credit*(-1);
			}

			lcd_command(0x01); // clear screen command
			lcd_command(0x80); // Set cursor to second line starting
			int tempNum1=sentVal;
			char tempChar1[30];
			int tempCo1=0;
			while(tempNum1!=0)
			{
				tempChar1[tempCo1]=(tempNum1%10)+'0';
				tempNum1=tempNum1/10;
				tempCo1++;
			}
			while(tempCo1!=0)
			{
				tempCo1--;
				UARTCharPut(UART1_BASE, tempChar1[tempCo1]);
			}
			if(credit==1)
			{
				UARTCharPut(UART1_BASE, 'z');
			}
			else
			{
				UARTCharPut(UART1_BASE, 'y');
			}

			send_time();

			UARTCharPut(UART1_BASE, 'l');


			initState();
		}

	}


	if(flagT == 1)
	{
		fppKey = inpChar; //Set last fully pressed key to this key
	}

}


/*
*
* Function Name: setupTimer
* Input: None
* Output: None
* Logic:Enables peripheral for timer0 and also enables interrupt.
* Example Call: setupTimer()
*
*/
void setupTimer()
{
	uint32_t ui32Period;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 1000);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);



	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
}



/*
*
* Function Name: main
* Input: None
* Output: None
* Logic: Enables various peripgerals and does basic initial settings.
* Example Call: get_cur_sec()
*
*/
void main(void)
{

	lcd_init();


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	setupRTC();


	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTEnable(UART1_BASE);


	delay = SysCtlClockGet() / 1000;
	initState();
	iteration = 0;



	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,
			GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);

	setupTimer();


	while (1) {}

}



