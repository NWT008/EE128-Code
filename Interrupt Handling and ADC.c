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
                0b11001110 // 9
				};

unsigned int count = 0;
unsigned long tens = 0;
unsigned long ones = 0;
unsigned long dec1 = 0;

unsigned long val_B = 0;
unsigned long MODE_SW  = 0;
unsigned long COUNT_DIR = 0;
unsigned long ADC_VAL = 0;
unsigned long display = 0;
unsigned long volts = 0;
unsigned long mVolts = 0;
unsigned long mappedC = 0;
unsigned long mappedD = 0;

uint16_t adc_read16(uint8_t ch)
	{
	    //ADC0_SC1A = 0x1A;
	    //while(ADC0_SC2 & ADC_SC2_ADACT_MASK);
		ADC0_SC1A = (uint32_t)(ch & 0x1F);
	    while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)) { }
	    return ADC0_RA;
	}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

void PORTA_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(PORTA_IRQn);// clear pending interrupt
	// Provided map()

	//reads PTB2 and PTB3
	val_B = GPIOB_PDIR;
	MODE_SW  = !((val_B >> 2) & 0x01);
	COUNT_DIR = !((val_B >> 3) & 0x01);
	//ADC_VAL = ((ADC_read16b() * 33) / 64000);

	if (dec1 == 0){
		dec1 = 1;
	}
	else{
		dec1 = 0;
	}

	if (MODE_SW == 1){
		ADC_VAL = adc_read16(0);
		display = map(ADC_VAL, 0, 65536, 0, 33);
		volts = display/10;
		mVolts = display % 10;
		GPIOD_PDOR = nums[volts];
        mappedD =
       	            ((nums[mVolts] & 0x3F)) |          // bits 0–5
       	            ((nums[mVolts] & 0x40) << 1) |     // bit 6 gets sent to 7
       	            ((nums[mVolts] & 0x80) << 1);      // bit 7 gets sent to 8
       	        								// or statements brings mappedD together
		GPIOC_PDOR = mappedD | dec1;
	} else if (MODE_SW == 0){
		if(COUNT_DIR == 0){
			if(count == 99){
				count = 0;;
			} else{
				count++;
			}
		} else if (COUNT_DIR == 1){
			if(count == 0){
				count = 99;
			} else{
				count--;
			}
		}

		tens = count/10;
		ones = count % 10;
		GPIOD_PDOR = nums[tens];
		mappedC =
			            ((nums[ones] & 0x3F)) |          // bits 0–5
			            ((nums[ones] & 0x40) << 1) |     // bit 6 gets sent to 7
			            ((nums[ones] & 0x80) << 1);      // bit 7 gets sent to 8
			        								// or statements brings mappedD together
		GPIOC_PDOR = mappedC | dec1;
	}



	PORTA_ISFR = (1<<1); //clears interrupt flag

}

int main(void)
{
	//Enables Ports for Clock Gate Control
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;

	//Configures ports for GPIO
	PORTA_PCR1 = 0xA0100UL;		//PORTA for GPIO & interrupt on Falling-Edge
	PORTB_GPCLR = 0x000C0103;   //PORTB for GPIO (input)
	PORTC_GPCLR = 0x01FF0100;   //PORTC for GPIO (output)
	PORTD_GPCLR = 0x00FF0100;   //PORTD for GPIO (output)
	ADC0_CFG1 = 0x0C;	

	//PORTD 0 - 7 0bgfedcbap
	//PORTC 0 - 8 (pin6 does not exist)

	ADC0_SC1A = 0x1F;

	//
	GPIOA_PDDR &= (1 << 1);
	GPIOB_PDDR = 0x00000000;
	GPIOC_PDDR = 0x000001FF;
	GPIOD_PDDR = 0x000000FF;

	GPIOC_PSOR = 0x1FF;
	GPIOD_PSOR = 0xFF;


	//Enables interrupts from PortA
	NVIC_EnableIRQ(PORTA_IRQn);

	for(;;){}
}

