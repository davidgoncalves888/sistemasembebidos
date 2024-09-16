#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

void delay(void) {
    volatile int i;

    for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init() {
    SIM->COPC = 0;
    SIM->SCGC5 |= 0x1000u;
    PORTD->PCR[5] |= 0x100u;
    GPIOD->PDDR |= 0x20u;
    GPIOD->PSOR;;
}

void led_green_toggle() {
    //
}

// LED_RED = PTE29
void led_red_init() {
    //
}

void led_red_toggle(void) {
    //
}

int main(void) {
    led_green_init();
    led_red_init();

    while (1) {
        led_green_toggle();
        delay();
        led_red_toggle();
    }

    return 0;
}
