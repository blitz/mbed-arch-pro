#include <mbed.h>
#include <math.h>

#include "i2c.hpp"
#include "compass.hpp"
#include "oled.hpp"

Serial pc(MBED_UARTUSB);

DigitalOut led1(LED1);
DigitalOut led2(LED2);

I2C i2c0(MBED_I2C0);

int main()
{
    led1 = 0;
    led2 = 0;

    i2c0.frequency(2000000);

    wait(3);

    pc.printf("Hello!\n");

    if (i2c_exists(i2c0, Compass::address)) {
        pc.printf("Compass is attached.\n");

        static Compass compass { i2c0 };

        for (int i = 0; i < 10; i++) {
            wait(0.2);
            pc.printf("Bearing %.0f\n", rad_to_deg(compass.bearing()));
        }
    }

    if (i2c_exists(i2c0, OLED::address)) {
        pc.printf("OLED found.\n");

        static OLED oled { i2c0 };
    }

    pc.printf("Done!\n");
}
