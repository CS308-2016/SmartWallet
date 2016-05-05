/*
 * lcd.c
 *
 *  Created on: 06-Mar-2016
 *  Author: saurav
 *
 *  Functions: lcd_portconfig(void), lcd_init(),
lcd_command(char cmd), lcd_char(char data), lcd_string(char *g), lcd_cursor (char row, char column), lcd_print (uint32_t value, int digits)
 *  Global Variables: None
 */
#include "commonheader.h"
#include "lcd.h"

#define lcd_delay    SysCtlDelay(67000*4)


void sendData(char cmnd)
{

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6, (cmnd%4)<<5);
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 , ((cmnd>>2)<<2)%64);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3, (cmnd>>6)<<2);


}


//#define lcd_put_data(x)  GPIOPinWrite(GPIO_PORTB_BASE,(0xFF), x)
#define lcd_put_data(x)  sendData(x)

#define lcd_set_RS() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_4),0x01<<4))
#define lcd_set_RW() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_5),0x01<<5))
#define lcd_set_EN() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_6),0x01<<6))
#define lcd_reset_RS() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_4),~(0x01<<4)))
#define lcd_reset_RW() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_5),~(0x01<<5)))
#define lcd_reset_EN() (GPIOPinWrite(GPIO_PORTC_BASE,(GPIO_PIN_6),~(0x01<<6)))


/*#define lcd_set_RS() (GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_4),0x01<<4))
#define lcd_set_RW() (GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_5),0x01<<5))
#define lcd_set_EN() (GPIOPinWrite(GPIO_PORTB_BASE,(GPIO_PIN_4),0x01<<4))
#define lcd_reset_RS() (GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_4),~(0x01<<4)))
#define lcd_reset_RW() (GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_5),~(0x01<<5)))
#define lcd_reset_EN() (GPIOPinWrite(GPIO_PORTB_BASE,(GPIO_PIN_4),~(0x01<<4)))*/

/*
 * lcd_portconfig();
 * Description: Sets LCD port pin as output
 * PORTB is connected to Datapins
 * PORTC pin4,pin5 and pin6 is connected to RS, RW and EN respectively. This connection is for TIVA booster board designed by WEL lab
 */

void lcd_portconfig(void)
{
	//Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
	//SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	//* * * * * * Setting Ouput for LCD * * * * * * * *
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,    GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,   GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6);

	// * * * * * * LCD_Control Pin* * * * * * *
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);


}

/*
 * lcd_init();
 * Description: Set ports pins as outputs. Initialize LCD in 8-bit mode.
 *
 * This should be the first function that must be called in main when using LCD
 */
void lcd_init()
{
	lcd_portconfig();

	lcd_reset_RW();
	lcd_reset_EN();

	lcd_command(0x38); // function set command
	lcd_command(0x0f); // display switch command
	lcd_command(0x06); // input set command
	lcd_command(0x01); // clear screen command
	lcd_command(0x80); // Set cursor to second line starting
}

/*
 * lcd_command()
 * Description: Sends command on databus
 * usage: lcd_command(0x01) - clears the display
 */
void lcd_command(char cmd)
{
	lcd_put_data(cmd);
	lcd_reset_RS();
	lcd_delay;
	lcd_set_EN();
	lcd_delay;
	lcd_reset_EN();
	lcd_delay;
}

/*
 *lcd_char()
 *Description: Print single character
 *Example lcd_char('A'); prints letter A
 */
void lcd_char(char data)
{
	lcd_put_data(data);
	lcd_set_RS();
	lcd_delay;
	lcd_set_EN();
	lcd_delay;
	lcd_reset_EN();
	lcd_delay;
}

/*
 * lcd_string()
 * Description: Print string
 * Example: lcd_string("Hello World");
 */
void lcd_string(char *g){
		while(*g)
			lcd_char(*g++);
	}

//
/*
 * Name: lcd_cursor (row, column). For setting cursor position in 16 by 2 lcd
 * Description: Position the LCD cursor at "row", "column"
 * row: 1,2
 * column: 1 to 16
 * Example: lcd_cursor(2,14) - Places cursor at 2nd line 14th column
 */

void lcd_cursor (char row, char column)
{
	switch (row)
	{
		case 1: lcd_command ((0x80 + column - 1));
		break;
		case 2: lcd_command (0xc0 + column - 1);
		break;
		default: break;
	}
}

void lcd_print (uint32_t value, int digits)
{
    uint32_t temp;
    uint32_t unit;
    uint32_t tens;
    uint32_t hundred;
    uint32_t thousand;
    uint32_t million;
    uint32_t million_10;
    uint32_t million_100;
	uint32_t flag=0;

	if(digits==7 || flag==1)
	{
		//temp = value/1000000
		million_100=value/1000000+48;
		lcd_char(million_100);
		flag=1;
	}

	if(digits==6 || flag==1)
	{
		temp = value/100000;
		million_10=temp%100000+48;
		lcd_char(million_10);
		flag=1;
	}


	if(digits==5 || flag==1)
	{
		temp = value/10000;
		million=temp%10+48;
		lcd_char(million);
		flag=1;
	}

	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_char(thousand);
		flag=1;
	}

	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_char(hundred);
		flag=1;
	}

	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_char(unit);
	}
	if(digits>7)
	{
		lcd_char('E');
	}
}
