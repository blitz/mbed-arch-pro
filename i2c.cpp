#include "i2c.hpp"
#include "printf.hpp"

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

// EOF
