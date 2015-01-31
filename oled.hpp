#pragma once

#include <mbed.h>
#include "printf.hpp"
#include "font8x8.hpp"

/// 96x96 pixel OLED display
/// http://www.displayfuture.com/Display/datasheet/controller/SSD1327.pdf
class OLED {
    I2C &i2c;


    /// Remember last row/col pair used for putc to avoid unnecessary
    /// commands.
    uint8_t last_row = 0xFF;
    uint8_t last_col = 0xFF;


public:
    constexpr static uint8_t address   = 0x3c << 1;
    constexpr static uint8_t cmd_mode  = 0x80;
    constexpr static uint8_t data_mode = 0x40;

    constexpr static uint8_t PIXELS         = 96;
    constexpr static uint8_t PIXEL_PER_CHAR = 8;

    constexpr static uint8_t ROWS = PIXELS / PIXEL_PER_CHAR;
    constexpr static uint8_t COLS = PIXELS / PIXEL_PER_CHAR;

    bool send(char mode, const char data[], size_t len)
    {
        for (size_t c = 0; c < len; c++) {
            const char snd[2] { mode, data[c] };
            if (i2c.write(address, snd, sizeof(snd)) != 0)
                return false;
        }

        return true;
    }

    void tribyte_cmd(uint8_t a, uint8_t b, uint8_t c)
    {
        char data[] = { a, b, c };
        send(cmd_mode, data, sizeof(data));

    }

    void set_column_address(uint8_t start, uint8_t end)
    {
        tribyte_cmd(0x15, start, end);
    }

    void set_row_address(uint8_t start, uint8_t end)
    {
        tribyte_cmd(0x75, start, end);
    }

    void putc(uint8_t row, uint8_t col, char c)
    {
        if (row >= ROWS or col > COLS) return;

        if (col != last_col) {
            set_column_address(8 + (col * PIXEL_PER_CHAR) / 2,
                               8 + (col * PIXEL_PER_CHAR + PIXEL_PER_CHAR) / 2 - 1);
            last_col = col;
        }

        if (row != last_row) {
            set_row_address   (row * PIXEL_PER_CHAR,
                               row * PIXEL_PER_CHAR + PIXEL_PER_CHAR - 1);
            last_row = row;
        }

        static const char pdata[4] = { 0x00, 0x0F, 0xF0, 0xFF };

        const unsigned char *font = &fontdata_8x8[c * 8];

        for (int i = 0; i < 8; i++) {
            const char dta[] = { pdata[(font[i] >> 6) & 0x3],
                                 pdata[(font[i] >> 4) & 0x3],
                                 pdata[(font[i] >> 2) & 0x3],
                                 pdata[(font[i] >> 0) & 0x3],
            };

            send(data_mode, dta, sizeof(dta));
        }
    }

    void puts(uint8_t row, uint8_t col, const char *str)
    {
        for (; *str; str++)
            putc(row, col++, *str);
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

        set_column_address(8, 8 + PIXELS/2 - 1);
        set_row_address   (0, PIXELS - 1);
        for (size_t x = 0; x < (48 * 96); x++) {
            char z = 0x00;
            send(data_mode, &z, sizeof(z));
        }
    }
};



// EOF
