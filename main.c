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

void led_green_on() {
    GPIOD->PCOR |= 1 << 5;
}

void led_green_off() {
    GPIOD->PSOR |= 1 << 5;
}

void led_red_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] |= PORT_PCR_MUX(1);
    GPIOE->PDDR |= 1 << 29;
    GPIOE->PSOR |= 1 << 29;
}

void led_red_toggle() {
    GPIOE->PTOR |= 1 << 29;
}

void led_red_on() {
    GPIOE->PCOR |= 1 << 29;
}

void led_red_off() {
    GPIOE->PSOR |= 1 << 29;
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

    int estado = 0;
    int estado_anterior = -1;
    while (1) {
        if (sw1_is_pressed()) {
            estado += 1;
            estado %= 4;
            while (sw1_is_pressed());
        }
        if (sw3_is_pressed()) {
            if (estado == 0) {
                estado = 1;
            } else if (estado == 1) {
                estado = 0;
            } else if (estado == 2) {
                estado = 3;
            } else {
                estado = 2;
            }
            while (sw3_is_pressed());
        }
        if (estado != estado_anterior) {
            estado_anterior = estado;
            switch (estado) {
                case 0:
                    led_green_off();
                    led_red_on();
                    break;
                case 1:
                    led_green_on();
                    led_red_off();
                    break;
                case 2:
                    led_green_on();
                    led_red_on();
                    break;
                case 3:
                    led_green_off();
                    led_red_off();
                    break;
            }
        }
    }
}
