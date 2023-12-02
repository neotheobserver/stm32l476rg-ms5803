#include "stm32l4xx.h" 
#include "i2c1.h"
#include "util.h"

void start_i2c1_cr2(uint8_t direction, uint8_t bytes){
	//unset previous values
	I2C1->CR2 &= ~(1 << 10 | 255 << 16 );
	I2C1->CR2 |= (direction << 10) | (bytes << 16);
	//set start bit
	I2C1->CR2 |= (1 << 13);
	//check if nack flag set
//	while (I2C1->ISR & (1<<4)){
//		// reset the nack flag
//		I2C1->ICR |= (1 << 4);
//		delay();
//		I2C1->CR2 |= (1 << 13);
//	}
//	I2C1->CR2 = 0;
//	I2C1->CR2 |= (0x76<<1)| (direction << 10) | (bytes << 16);
//	I2C1->CR2 |= (1 << 13);
}

void stop_i2c1_cr2(){
	//set stop bit
	I2C1->CR2 |= (1 << 14);
}

void update_address(uint8_t address){
	//unset the bit 1-7
	I2C1->CR2 &= ~(127 << 1);
	//set the address(its supposed to be byte 1 to 7(7 byte addressing))
	I2C1->CR2 |= (address << 1);
}

void init_i2c1(void){
	//init gpio
	//Configure I2C -> GPIOB port 9 for sda port 8 for scl
	RCC->AHB2ENR |= (1ul << 1);
	GPIOB->MODER &= ~((3ul << 18) | (3ul << 16));
	GPIOB->MODER |= ((2ul << 18)|(2ul << 16));
	// Set open drain mode
	GPIOB->OTYPER |= ((1ul << 9)|(1ul << 8));
	// Set pull up
	GPIOB->PUPDR &= ~((3ul << 18)| (3ul << 16));
	GPIOB->PUPDR |= ((1ul << 18)|(1ul << 16));
	// Set AF4 for I2C1 SDA AND SCL ports
	GPIOB->AFR[1] &= ~((15ul) | (15ul << 4));
	GPIOB->AFR[1] |= ((4ul) |(4ul << 4));
	
	// Clock for I2C1 RCC_APB1ENR1_I2C1EN
	RCC->APB1ENR1 |= (1ul << 21);
	
	
	//init cr1 register
	//I2C initialization
	//Clear pe bit in i2c_Cr1 set dnf digital noise filter to off
	I2C1->CR1 &= ~(1ul);
	//set analog noise filter to on
	I2C1->CR1 &= ~(1ul << 12);
	//set digital filter to on
	I2C1->CR1 |= (15ul << 8);
	//config isc1->timngr (config taken from reference manual with 8mhz clock for 100khz)
	I2C1->TIMINGR |= (0x13 | 0xF << 8 | 0x2 << 16 | 0x4 << 20 | 0x1 << 28);

	// configure nostretch i.e. stretching is off
//	I2C1->CR1 |= (1ul << 17);
	// Turn on the I2C1 perpheral I2C_CR1_PE
	I2C1->CR1 |= (1ul);
	
	//init cr2 register(everything works for 7 bit addressing only)
	//-Addressing mode add10 or 7  (bit 11 to 0 for 7)
	//-transfer direction-rd_wrn (nit 10 to 0 for write)
	//-finally the start bit to generate start condition(this must be at the end)
	//  Can configure autoend by setting I2C_CR2_AUTOEND as well
	// Configure byte to transmit i.e. 1 at I2C_CR2_NBYTES_Pos
	// Set slave address  at I2C_CR2_SADD_POS (needs to be 7 bit, bit 0,8,9 are ignored)
	//address mode bit to 0
	I2C1->CR2 &= ~(1 << 11);
	update_address(0x76);
	delay();
}
