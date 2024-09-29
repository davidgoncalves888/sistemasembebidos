#include "MKL46Z4.h"

void delay(int cycles) {
    volatile int i;

    for (i = 0; i < cycles; i++);
}

void led_green_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] |= PORT_PCR_MUX(1);
    GPIOD->PDDR |= 0x20u;
    GPIOD->PSOR |= 1 << 5;
}

void led_green_toggle() {
    GPIOD->PTOR |= 1 << 5;
}

// LED_RED = PTE29
void led_red_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] |= PORT_PCR_MUX(1);
    GPIOE->PDDR |= 1 << 29;
    GPIOE->PSOR |= 1 << 29;
}

void led_red_toggle() {
    GPIOE->PTOR |= 1 << 29;
}

void switch1_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] |= PORT_PCR_PS(1) | PORT_PCR_PE(1) | PORT_PCR_MUX(1);
    GPIOC->PDDR &= ~(1 << 3);
}

int sw1_is_pressed(void) {
    return (!(GPIOC->PDIR & (1 << 3)));
}

void switch3_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
    GPIOC->PDDR &= ~(1 << 12);
}

int sw3_is_pressed(void) {
    return !(GPIOC->PDIR & (1 << 12));
}

int main() {
    SIM->COPC = 0;
    led_green_init();
    led_red_init();
    switch1_init();
    switch3_init();

    while (1) { ;
        if (sw1_is_pressed()) {
            led_green_toggle();
            while (sw1_is_pressed());
        }
        if (sw3_is_pressed()) {
            led_red_toggle();
            while (sw3_is_pressed());
        }
    }
}
