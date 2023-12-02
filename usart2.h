#ifndef __USART2_H
#define __USART2_H

#include <stdint.h>
void config_usart2();
void checkInput();
void printData(uint8_t *out, int len);

#endif