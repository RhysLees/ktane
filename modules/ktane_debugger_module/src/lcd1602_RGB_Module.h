#ifndef _LCD1602_RGB_MODULE_h_
#define _LCD1602_RGB_MODULE_h_
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <stdio.h>
#include <stdlib.h>
/*!
 *   Device I2C Arress
 */
#define LCD_ADDRESS (0x7c >> 1)
#define RGB_ADDRESS (0xc0 >> 1)

extern uint8_t _showfunction;
/*!
 *  color define
 */
#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3

#define REG_RED 0x04   // pwm2
#define REG_GREEN 0x03 // pwm1
#define REG_BLUE 0x02  // pwm0

#define REG_MODE1 0x00
#define REG_MODE2 0x01
#define REG_OUTPUT 0x08

/*!
 *   commands
 */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/*!
 *   flags for display entry mode
 */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/*!
 *   flags for display on/off control
 */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/*!
 *   flags for display/cursor shift
 */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/*!
 *   flags for function set
 */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x8DOTS 0x00

void LCD1602RGB_init(uint8_t col, uint8_t row);
void begin(uint8_t col, uint8_t row);
void command(uint8_t cmd);
void display();
void clear();
void setReg(uint8_t reg, uint8_t data);
void setRGB(uint8_t r, uint8_t g, uint8_t b);
void setColorWhite();
void write_char(uint8_t data);
void send_string(const char *str);
void setCursor(uint8_t col, uint8_t row);
#endif