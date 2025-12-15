# EE128 Lab Schematics

Lab 2 (Simple Parallel I/O):
In this lab we will expand, implementing the parallel circuit to create a binary counter and a one-hot rotator. 
We will use hardware elements including the K64F board, two 8 led strips, a DIP switch and resistors. In software, we will program the K64F for I/O as well as the logic for the counter and rotator. 
The DIP switch pin 1 and 2 positions will determine whether the counter is counting up or down, and whether the rotator is moving right to left or left to right. 
By the end of this lab, we should be able to enable the K64F for GPIO components and understand the infrastructure of the microcontroller.

![image.alt](https://github.com/NWT008/EE128-Code/blob/main/Lab2%20pic.jpg?raw=true)

Lab 3 (Interrupt Handling & ADC):
This program begins with system initialization, setting the K64F ports for I/O. We also configured our ADC ports to read the incoming voltage. Then we read port B to determine which mode we are working in. If the first switch is set to 0, we will read and output the voltage that is input to the ADC. Otherwise, if the first switch is set to 1, we will be in counting mode. The second switch correlates to CNT_DIR and only has an effect on the circuit when set to 1. If the second switch is set to 0, then the counter will count up. If the switch is set to 1, the counter will count down. Both will rollover if the max/min is hit. The second display's decimal point will blink following the voltage input to the clock port.

![image.alt](https://github.com/NWT008/EE128-Code/blob/main/lab3%20pic.png?raw=true)

Lab 4 (Timer & PWM Operations):
This program begins with system initialization and declaring our required variables. It then reads a time of when the rising edge is detected in PINC. When the edge is detected, a time value is stored and the falling edge detector waits. When the falling edge triggers, the timestamp is stored. After storing that variable, it detects another rising edge. With this information, it is able to detect the period and length the signal is set to on. It uses the formula (100 * pulse width)/period to calculate the duty cycle, which is then output to the seven segment displays. 

![image.alt](https://github.com/NWT008/EE128-Code/blob/main/lab4%20pic.png?raw=true)

Lab 5 (Stepper Motor):
The bread board has two terminals for power. We connected different DC terminals to each power terminal, one that supplies 9v and one that supplies 5v. The MCU is directly connected to the L298N input terminals, which outputs the signals needed to turn the stepper motor. This tells the motor how to turn and specifies speed and direction. The DIP switch is connected to the input terminals. This terminal detects whether the signal is high or low and the MCU proceeds to make decisions based on our program code. Enable 1 and enable 2 are always high which allows the motor to spin all the time.

![image.alt](https://github.com/NWT008/EE128-Code/blob/main/lab5%20pic.png?raw=true)
