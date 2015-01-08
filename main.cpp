#include "mbed.h"

#include <math.h>

namespace {

    static const float PI = 3.141592653589793238;

    DigitalOut led1(LED1);
    DigitalOut led2(LED2);

    Serial pc(MBED_UARTUSB);

    bool i2c_exists(I2C &i2c, uint8_t addr)
    {
        return i2c.write(addr, nullptr, 0) == 0;
    }

    void scan_i2c(const char *name, I2C &i2c)
    {
        pc.printf("\nScanning %s...\n", name);

        for (int addr = 1; addr < 127; addr++) {
            if (i2c_exists(i2c, addr << 1)) {
                pc.printf("%s: %02x\n", name, addr);
            }
        }
    }

    float rad_to_deg(float r)
    {
        return r * (180 / PI);
    }

    struct Vector {
        int16_t x, y, z;
    };

    class Compass {
        I2C &i2c;

        uint16_t be_to_signed(char a, char b)
        {
            return int16_t((unsigned)a << 8 | (unsigned)b);
        }

    public:
        constexpr static uint8_t address = 0x1e << 1;

        bool read(Vector &v)
        {
            char   data[6];

            char reg = 3;

            int err1 = i2c.write(address, &reg, 1);
            int err2 = i2c.read(address, data, sizeof(data));

            if (err1 or err2) {
                return false;
            } else {
                v.x = be_to_signed(data[0], data[1]);
                v.z = be_to_signed(data[2], data[3]);
                v.y = be_to_signed(data[4], data[5]);

                return true;
            }
        }

        float bearing()
        {
            Vector v;

            if (read(v)) {
                return atan2f(v.y, v.x);
            } else {
                return 0;
            }
        }

        Compass(I2C &i2c)
            : i2c(i2c)
        {
            // Continuous updates at 10 Hz
            static const char init[] = { 0x02, 0x00 };
            i2c.write(address, init, sizeof(init));
        }

    };

    /// 96x96 pixel OLED display
    /// http://www.displayfuture.com/Display/datasheet/controller/SSD1327.pdf
    class OLED {
        I2C &i2c;

    public:
        constexpr static uint8_t address   = 0x3c << 1;
        constexpr static uint8_t cmd_mode  = 0x80;
        constexpr static uint8_t data_mode = 0x40;

        bool send(char mode, const char data[], size_t len)
        {
            for (size_t c = 0; c < len; c++) {
                const char snd[2] { mode, data[c] };
                if (i2c.write(address, snd, sizeof(snd)) != 0)
                    return false;
            }

            return true;
        }

        OLED(I2C &i2c)
            : i2c(i2c)
        {
            static const char init[] = { 0xFD, 0x12, 0xAE, 0xA8, 0x5F, 0xA1,
                                         0x00, 0xA2, 0x60, 0xA0, 0x46, 0xAB,
                                         0x01, 0x81, 0x53, 0xB1, 0x51, 0xB3,
                                         0x01, 0xB9, 0xBC, 0x08, 0xBE, 0x07,
                                         0xB6, 0x01, 0xD5, 0x62, 0xA4, 0x2E,
                                         0xAF };

            if (not send(cmd_mode, init, sizeof(init))) {
                pc.printf("OLED init failed.\n");
            }

            wait_ms(100);

            static const char init2[] = { 0xA0, 0x42,
                                          0x15, 0x08, 0x37, 0x75, 0x00, 0x5f  };

            if (not send(cmd_mode, init2, sizeof(init2))) {
                pc.printf("OLED init2 failed.\n");
            }

            pc.printf("OLED initialized.\n");

            for (unsigned i = 0; i < 20; i++)
                for (size_t x = 0; x < (48 * 96); x++) {
                    char z = 0x11 * ((i + x) & 0xF);
                    send(data_mode, &z, sizeof(z));
                }
        }
    };


    I2C i2c0(MBED_I2C0);
}

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
