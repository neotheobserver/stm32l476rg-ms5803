#ifndef __MS5803_H
#define __MS5803_H

#include <stdint.h>
void config_coefficient(void);
void single_write(uint8_t command);
void read_coef(uint8_t* tempdata);
void read_adc(uint8_t* tempDataReadings);
void update_pressure_temperature(int32_t* values);
void reset_ms5803(void);
#endif