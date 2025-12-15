#include "fsl_device_registers.h"

void software_delay(unsigned long delay) {
    while (delay--) { __NOP(); }
}

int main(void) {
    //Enable port clocks
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // Inputs (PTB2 & PTB3)
    PORTB_GPCLR = 0x000C0103;

    // Outputs (PTC0–8, PTD0–7)
    PORTC_GPCLR = 0x01FF0100;  // PTC0–8 -> GPIO
    PORTD_GPCLR = 0x00FF0100;  // PTD0–7 -> GPIO

    GPIOB_PDDR = 0x00000000;  // Port B inputs
    GPIOC_PDDR = 0x000001FF;  // Port C outputs
    GPIOD_PDDR = 0x000000FF;  // Port D outputs

    //Sets everything OFF
    GPIOC_PSOR = 0x1FF;
    GPIOD_PSOR = 0xFF;

    unsigned long Delay = 0x100000;

    volatile unsigned long count = 0;
    volatile unsigned long rot = 0x01;

    while (1) {
        software_delay(Delay);

        //reads PTB2 and PTB3
        unsigned long val_B = GPIOB_PDIR;
        unsigned long COUNT_UP  = !((val_B >> 2) & 0x01);
        unsigned long ROT_RIGHT = !((val_B >> 3) & 0x01);

        // Counter
        if (COUNT_UP) {
            count = (count == 255) ? 0 : (count + 1); // sets to 0, else counts up 1
        } else {
            count = (count == 0) ? 255 : (count - 1); // sets to 255, else counts down 1
        }

        // Rotator
        if (rot == 0) rot = 0x01; // safety seed
        if (ROT_RIGHT) {
            rot = (rot == 0x01) ? 0x80 : (rot >> 1); // right
        } else {
            rot = (rot == 0x80) ? 0x01 : (rot << 1); // left
        }

        // Map 8-bit counter to PD0–5, PD7, PD8 (skip PD6)
        unsigned long mappedD =
            ((count & 0x3F)) |          // bits 0–5
            ((count & 0x40) << 1) |     // bit 6 gets sent to 7
            ((count & 0x80) << 1);      // bit 7 gets sent to 8
        								// or statements brings mappedD together

        GPIOC_PDOR = (~mappedD) & 0x1FF;  // mask 9 bits (PTC0–PTC8)

        GPIOD_PDOR = (~rot) & 0xFF; // mask 8 bits, 0 is on
    }
}
