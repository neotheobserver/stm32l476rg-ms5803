#include "stm32l4xx.h" 
#include "stdio.h"
#include "util.h"
#include "usart2.h"
#include "i2c1.h"
#include "ms5803.h"

void config_clock(void){
	// Enable msi clock at 8mhz for sys clock
	RCC->CR |= (1);
	while(!(RCC->CR & (1 << 1)));
	// set msi range from cr itself
	RCC->CR |= (1 << 3);
	// set msi frequency 8
	RCC->CR |= (7 << 4);
	//SEt msi as sysclcksrc
	RCC->CFGR &= ~(3);
	SystemCoreClockUpdate();
}

void trace(char * arg) {
	while (*arg) {
		ITM_SendChar(*arg++);
	}
}

int main(void){
	config_clock();
	config_usart2();
	init_i2c1();
	start_i2c1_cr2(0, 1);
	delay();
//	reset_ms5803();
	config_coefficient();
	trace("Sensor Readings\r\n");
	while(1){
		delay();
		int32_t values[2] = {0,0};
		update_pressure_temperature(values);
		char payload[64] = { 0 };
		int len = sprintf(payload, (const char * )"Temperature: %0.2f\tPressure: %0.2f\r\n", values[0]*0.01, values[1]/137.5763); // the pressure is off by ~13 time (not sure why?)
		printData((uint8_t *)payload, len);
		trace(payload);
	}
	return 0;
}
