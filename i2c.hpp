#pragma once

#include <mbed.h>


bool i2c_exists(I2C &i2c, uint8_t addr);
void scan_i2c(const char *name, I2C &i2c);

// EOF
