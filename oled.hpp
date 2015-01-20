#pragma once

#include <mbed.h>
#include "printf.hpp"

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



// EOF
