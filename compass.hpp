#pragma once

#include "i2c.hpp"
#include "math.hpp"

class Compass {
    I2C &i2c;

    uint16_t be_to_signed(char a, char b)
    {
        return int16_t((unsigned)a << 8 | (unsigned)b);
    }

    struct Vector {
        int16_t x, y, z;
    };

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


// EOF
