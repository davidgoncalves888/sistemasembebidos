#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
#define LED_GREEN PTD5 (pin 98)
#define LED_RED PTE29 (pin 26)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void disable_watchdog() {
    SIM->COPC = 0;
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

void delay(void) {
    volatile int i;

    for (i = 0; i < 3000000; i++);
}

int main(void) {
    disable_watchdog();
    led_green_init();
    led_red_init();
    switch1_init();
    switch3_init();
    irclk_ini();
    lcd_ini();

    int hit = 0, misses = 0;
    bool stop_loop = false;
    lcd_display_dec(666);
    lcd_display_time(12, 24);

    // 'Random' sequence :-)
    volatile unsigned int sequence = 0x32B14D98,
            index = 0;

    while (index < 32) {
        if (sequence & (1 << index)) { //derecho
            led_green_on();
            while (!stop_loop) {
                if (sw1_is_pressed()) {
                    hit++;
                    stop_loop = true;
                    while (sw1_is_pressed());
                } else if (sw3_is_pressed()) {
                    misses++;
                    stop_loop = true;
                    while (sw3_is_pressed());
                }
                lcd_display_time(hit, misses);
            }
            stop_loop = false;
            led_green_off();
        } else { //izquierdo
            led_red_on();
            while (!stop_loop) {
                if (sw1_is_pressed()) {
                    misses++;
                    stop_loop = true;
                    while (sw1_is_pressed());
                } else if (sw3_is_pressed()) {
                    hit++;
                    stop_loop = true;
                    while (sw3_is_pressed());
                }
                lcd_display_time(hit, misses);
            }
            stop_loop = false;
            led_red_off();
        }
        index++;
    }
    int i = 20;
    while (i != 0) {
        i--;
        lcd_display_time(hit, misses);
        delay();
        lcd_display_time(0, 0);
        delay();
    }
    while (1) {
    }

    return 0;
}
