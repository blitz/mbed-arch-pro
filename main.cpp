#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial pc(MBED_UARTUSB);

I2C i2c0(MBED_I2C0);
I2C i2c1(MBED_I2C1);

int main()
{
    //Thread thread(led2_thread);

    led1 = 0;
    led2 = 1;

    while (true) {
        wait(0.5);
        pc.printf("Ping!\n");
        led1 = !led1;
    }
}
