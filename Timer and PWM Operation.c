#include "fsl_device_registers.h"

int nums[10] = {0b01111110, // 0
                0b00001100, // 1
                0b10110110, // 2
                0b10011110, // 3
                0b11001100, // 4
                0b11011010, // 5
                0b11111010, // 6
                0b00001110, // 7
                0b11111110, // 8
                0b11001110  // 9
				};

unsigned long tens = 0;
unsigned long ones = 0;
unsigned long mappedC = 0;
volatile unsigned int nr_overflows = 0;   // made volatile (used in ISR + main)
unsigned int t1, t2, t3, pulse_width, period, duty_cycle, T1, T2, T3, o1, o2, o3;

void FTM3_IRQHandler(void){
	// Increment overflow count only when TOF is set, then clear TOF
	if (FTM3_SC & 0x80){
		nr_overflows++;
		FTM3_SC &= 0x7F;
	}
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

int main(void)
{
	//Enables Ports for Clock Gate Control
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;

	//Configures ports for GPIO
	PORTC_GPCLR = 0x01FF0100;   //PORTC for GPIO (output)
	PORTC_PCR10 = 0x300;
	PORTD_GPCLR = 0x00FF0100;   //PORTD for GPIO (output(

	FTM3_MODE = 0x5;
	FTM3_MOD = 0XFFFF;
	FTM3_SC = 0x0E;

	//PORTD 0 - 7 0bgfedcbap
	//PORTC 0 - 8 (pin6 does not exist)

	//
	GPIOC_PDDR = 0x000001FF;
	GPIOD_PDDR = 0x000000FF;

	GPIOC_PSOR = 0x1FF;
	GPIOD_PSOR = 0xFF;

	NVIC_EnableIRQ(FTM3_IRQn);
	FTM3_SC |= 0x40;

	while (1){
		// reset 16-bit counter and our software overflow baseline
		FTM3_CNT = 0; nr_overflows = 0;

		//wait high
		FTM3_C6SC = 0x4;
		while(!(FTM3_C6SC & 0x80));
		FTM3_C6SC &= ~(1<<7);
		t1 = FTM3_C6V;
		o1 = nr_overflows;

		//wait low
		FTM3_C6SC = 0x8;
		while (!(FTM3_C6SC & 0x80));
		FTM3_C6SC = 0;
		t2 = FTM3_C6V;
		o2 = nr_overflows;

		//wait high
		FTM3_C6SC = 0x4;
		while(!(FTM3_C6SC & 0x80));
		FTM3_C6SC &= ~(1<<7);
		t3 = FTM3_C6V;
		o3 = nr_overflows;

		// build 32-bit extended timestamps from overflow snapshots + 16-bit captures
		T1 = (o1 << 16) | t1;
		T2 = (o2 << 16) | t2;
		T3 = (o3 << 16) | t3;

		// compute period and pulse width using the same extended timeline
		period = T3 - T1;
		pulse_width = T2 - T1;

		duty_cycle = (100 * pulse_width) / period;

		tens = duty_cycle / 10;
		ones = duty_cycle % 10;

		GPIOD_PDOR = nums[tens];
		mappedC =
				((nums[ones] & 0x3F)) |     // bits 0–5
		   ((nums[ones] & 0x40) << 1) |     // bit 6 gets sent to 7
		   ((nums[ones] & 0x80) << 1);      // bit 7 gets sent to 8
					  	  	  	  	  	  	// or statements brings mappedD together
		GPIOC_PDOR = mappedC;

		for (int i = 0; i < 1000000; i++);
	}
}

