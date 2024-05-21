#include "lcd1602_RGB_Module.h"
#include "main.h"

uint8_t _showfunction;
uint8_t _showcontrol;
uint8_t _showmode;
uint8_t _numlines;
uint8_t _currline;
void LCD1602RGB_init(uint8_t col, uint8_t row)
{
    i2c_init(DISPLAY_I2C_PORT, 100000);
    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
    _showfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(col,row);

}

void begin(uint8_t col, uint8_t lines)
{
    if (lines > 1) {
        _showfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _currline = 0;
    
 
    ///< SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    ///< according to datasheet, we need at least 40ms after power rises above 2.7V
    ///< before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    sleep_ms(50);


    ///< this is according to the hitachi HD44780 datasheet
    ///< page 45 figure 23

    ///< Send function set command sequence
    command(LCD_FUNCTIONSET | _showfunction);
    sleep_ms(10);  // wait more than 4.1ms
	
	///< second try
    command(LCD_FUNCTIONSET | _showfunction);
    sleep_ms(10);

    ///< third go
    command(LCD_FUNCTIONSET | _showfunction);

    //command(LCD_FUNCTIONSET | _showfunction);


    ///< turn the display on with no cursor or blinking default
    _showcontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    ///< clear it off
    clear();

    ///< Initialize to default text direction (for romance languages)
    _showmode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    ///< set the entry mode
    command(LCD_ENTRYMODESET | _showmode);
    
    
    ///< backlight init
    setReg(REG_MODE1, 0);
    sleep_ms(2);
    ///< set LEDs controllable by both PWM and GRPPWM registers
    setReg(REG_OUTPUT, 0xFF);
    sleep_ms(2);
    ///< set MODE2 values
    ///< 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
    setReg(REG_MODE2, 0x20);
    
    setColorWhite();
}

void command(uint8_t cmd)
{
    uint8_t val[2] = {0x80,cmd};
    i2c_write_blocking(DISPLAY_I2C_PORT,LCD_ADDRESS,val,2,false);
}

void display()
{
    command(LCD_DISPLAYCONTROL | _showcontrol);
    sleep_ms(2);
}

void clear()
{
    command(LCD_CLEARDISPLAY);
    sleep_ms(2);
}

void setReg(uint8_t reg,uint8_t data)
{
    uint8_t val[2] = {reg,data};
    i2c_write_blocking(DISPLAY_I2C_PORT,RGB_ADDRESS,val,2,false);
}

void setRGB(uint8_t r,uint8_t g,uint8_t b)
{
    setReg(REG_RED,r);
    setReg(REG_GREEN,g);
    setReg(REG_BLUE,b);
}
void setColorWhite()
{
    setRGB(0,0,255);
}

void write_char(uint8_t data)
{
    uint8_t val[2] = {0x40,data};
    i2c_write_blocking(DISPLAY_I2C_PORT,LCD_ADDRESS,val,2,false);
}

void send_string(const char *str)
{
	uint8_t i;
	for(i = 0; str[i] != '\0';i++)
		write_char(str[i]);
}
void setCursor(uint8_t col, uint8_t row)
{
    if(row == 0)
    {
        col |= 0x80;
    }
    else
    {
        col |= 0xc0;
    }
    command(col);
}