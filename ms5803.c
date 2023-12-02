#include "stm32l4xx.h" 
#include "ms5803.h"
#include "i2c1.h"
#include "util.h"

uint16_t coeff[6];

void reset_ms5803(){
	//ms5803-14ba reset
	single_write(0x1E);
}

void config_coefficient(void){
	uint8_t tempdata[2] = {0,0};
	uint8_t count = 0;

	single_write(0xA2);
	read_coef(tempdata);
	coeff[count] = (tempdata[0] << 8)+tempdata[1];
	count++;
	
	single_write(0xA4);
	read_coef(tempdata);
	coeff[count] = ((tempdata[0] << 8)|tempdata[1]);
	count++;
	
	single_write(0xA6);
	read_coef(tempdata);
	coeff[count] = ((tempdata[0] << 8)|tempdata[1]);
	count++;
	
	single_write(0xA8);
	read_coef(tempdata);
	coeff[count] = ((tempdata[0] << 8)|tempdata[1]);
	count++;
	
	single_write(0xAA);
	read_coef(tempdata);
	coeff[count] = (tempdata[0] << 8)|tempdata[1];
	count++;
	
	single_write(0xAC);
	read_coef(tempdata);
	coeff[count] = ((tempdata[0] << 8)|tempdata[1]);
}


void single_write(uint8_t command){
	//change direction to write and write 1 bytes of data
	// write=0 read=1,no of bytes
	start_i2c1_cr2(0, 1);
	I2C1->TXDR = command;
	while(!(I2C1->ISR &1));
	stop_i2c1_cr2();
}

void read_coef(uint8_t* tempdata){
	//change direction to read and read 2 bytes
	// write=0 read=1,no of bytes
	start_i2c1_cr2(1, 2);
	while(!(I2C1->ISR & (1 << 2)));
	*tempdata = I2C1->RXDR;
	while(!(I2C1->ISR & (1 << 2)));
	*(tempdata+1) = I2C1->RXDR;	
	stop_i2c1_cr2();
}

void read_adc(uint8_t* tempDataReadings){
	// write=0 read=1,no of bytes
	start_i2c1_cr2(1, 3);
	while(!(I2C1->ISR & (1 << 2)));
	*(tempDataReadings) = I2C1->RXDR;
	while(!(I2C1->ISR & (1 << 2)));
	*(tempDataReadings+1) = I2C1->RXDR;	
	while(!(I2C1->ISR & (1 << 2)));
	*(tempDataReadings+2) = I2C1->RXDR;	
	stop_i2c1_cr2();
}

void update_pressure_temperature(int32_t* values){
	//convert digital pressure at OSR=512
		single_write(0x42);
		delay();
	
		//initiate adc read
		single_write(0x00);
	
		uint8_t tempdataReadings[3] = {0,0,0};
		read_adc(tempdataReadings);
		//D1 reading
		uint32_t pressureTemp = (tempdataReadings[0] << 16)| (tempdataReadings[1] << 8) | tempdataReadings[2];
	
		//convert digital temperature at OSR=512
		single_write(0x52);
		delay();
	
		single_write(0x00);
		read_adc(tempdataReadings);
		//D2 reading
		uint32_t temperatureTemp = (tempdataReadings[0] << 16)| (tempdataReadings[1] << 8) | tempdataReadings[2];
	
		 //dT = D2 - T REF = D2 - C5 * 2^8 
		int32_t dT = (temperatureTemp - (coeff[4] << 8));
		//TEMP = 20°C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
		*values = 2000 + ((dT*coeff[5] >> 23));
		
		//OFF = OFFT1 + TCO * dT = C2 * 2^16 + (C4 * dT ) / 2^7 
		int64_t offset = (coeff[1] << 16) + ((coeff[3] * dT) >> 7);
		//SENS = SENS T1 + TCS * dT = C1 * 2^15 + (C3 * dT ) / 2^8 
		int64_t sensitivity = (coeff[0] << 15) + ((coeff[2] * dT) >> 8);
		//P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15
		*(values+1) = (((pressureTemp * sensitivity) >> 21) - offset) >> 15;
}