#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"

#include "main.h"

extern "C"
{
    #include "lcd1602_RGB_Module.h"
}

void init()
{
    stdio_init_all();
}

void loop()
{
    uint8_t r, g, b;
    float t = 0.0;
    LCD1602RGB_init(16, 2);

    while (1)
    {
        r = abs((sin(t)) * 255);
        g = abs((sin(t + 60)) * 255);
        b = abs((sin(t + 120)) * 255);
        t = t + 10;
        setRGB(r, g, b);
        setCursor(0, 0);
        send_string("Waveshare");
        setCursor(0, 1);
        send_string("Hello,World!");
        sleep_ms(300);
    }
}

int main()
{
    init();

    loop();
}