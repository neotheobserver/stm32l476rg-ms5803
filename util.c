#include "util.h"
#include "stm32l4xx.h" 

void delay(){
	for(int i=0; i < 200000; i++){
		__NOP();
	}
}