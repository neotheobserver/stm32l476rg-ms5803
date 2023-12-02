#ifndef __I2C2_H
#define __I2C2_H

#include <stdint.h>
void start_i2c1_cr2(uint8_t direction, uint8_t bytes);
void stop_i2c1_cr2(void);
void init_i2c1(void);
void update_address(uint8_t address);
#endif