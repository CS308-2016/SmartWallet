/*
 * lcd.h
 *
 *  Created on: 06-Mar-2016
 *  Author: saurav
 *
 *  Description: use #include "lcd.h" in any program that needs using lcd
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_init(void);
void lcd_command(char);
void lcd_char(char);
void lcd_string(char *);
void lcd_print(uint32_t, int);
void lcd_cursor(char,char);


#endif /* LCD_H_ */
