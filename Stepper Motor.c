#include "fsl_device_registers.h"

void software_delay(volatile unsigned long delay)
{
    while (delay > 0) delay--;
    return;
}

// PD0=A1, PD1=B1, PD2=A2, PD3=B2
// ENA(PD4)+ENB(PD5)=0x30
const uint8_t step_pattern[4] = {
    0x33, // A1=1, B1=1, A2=0, B2=0
    0x36, // A1=0, B1=1, A2=1, B2=0
    0x3C, // A1=0, B1=0, A2=1, B2=1
    0x39  // A1=1, B1=0, A2=0, B2=1
};

int dir_input = 0;
int spd_input = 0;

void main(void) {
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configure PD0-PD5 as GPIO output
    PORTD_PCR0 = 0x100;
    PORTD_PCR1 = 0x100;
    PORTD_PCR2 = 0x100;
    PORTD_PCR3 = 0x100;
    PORTD_PCR4 = 0x100; // ENA
    PORTD_PCR5 = 0x100; // ENB

	GPIOD_PDDR |= (1 << 0);
	GPIOD_PDDR |= (1 << 1);
	GPIOD_PDDR |= (1 << 2);
	GPIOD_PDDR |= (1 << 3);
	GPIOD_PDDR |= (1 << 4);
	GPIOD_PDDR |= (1 << 5);

    PORTB_PCR2 = 0x103; // MUX=1 (GPIO) + PE=1 (Pull Enable) + PS=1 (Pull-Up)
    PORTB_PCR3 = 0x103; // MUX=1 (GPIO) + PE=1 (Pull Enable) + PS=1 (Pull-Up)
    GPIOB_PDDR &= ~((1 << 2) | (1 << 3)); // Set PB2/PB3 as input

    int step = 0;

    dir_input = (GPIOB_PDIR & 0b1000) >> 3;
    spd_input = (GPIOB_PDIR & 0b0100) >> 2;

    for (;;) {
    	dir_input = (GPIOB_PDIR & 0b1000) >> 3;
    	spd_input = (GPIOB_PDIR & 0b0100) >> 2;
        	GPIOD_PDOR = step_pattern[step]; //motor out
	
	// program logic
    	if (dir_input == 1){ // default
    		step = (step + 1) % 4;
    	}
    	else{ // switched
    		step = (step + 3) % 4;
    	}

    	if (spd_input == 0){ // switched = slow
    		software_delay(90000);
    	}
    	else { // spd_input == 1 default = fast
    	software_delay(10000);
    	}
    }
}
