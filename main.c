#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"

#define SW1_PIN 3
#define SW3_PIN 12


// Enable IRCLK (Internal Reference Clock)
void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
}

void disable_watchdog() {
    SIM->COPC = 0;
}

void delay(void) {
    volatile int i;
    for (i = 0; i < 3000000; i++);
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


/*void switch1_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] |= PORT_PCR_PS(1) | PORT_PCR_PE(1) | PORT_PCR_MUX(1) | PORT_PCR_IRQC(0b1010) | PORT_PCR_ISF(1);  // Interrupt on falling edge
    GPIOC->PDDR &= ~(1 << 3);
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);  // Enable PORTC interrupt in NVIC
}*/
void switch_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    /* Configure pin as GPIO */
    PORTC->PCR[SW1_PIN] |= PORT_PCR_MUX(1);
    PORTC->PCR[SW3_PIN] |= PORT_PCR_MUX(1);
    /* Enable internal pullup resistor */
    PORTC->PCR[SW1_PIN] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC->PCR[SW3_PIN] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    /* Interrupt on falling edge */
    PORTC->PCR[SW1_PIN] |= PORT_PCR_IRQC(0x0A);
    PORTC->PCR[SW3_PIN] |= PORT_PCR_IRQC(0x0A);

    /* Enable IRQ */
    NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
    NVIC_SetPriority(PORTC_PORTD_IRQn, 1);
}

void switch3_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[12] |= PORT_PCR_PS(1) | PORT_PCR_PE(1) | PORT_PCR_MUX(1) | PORT_PCR_IRQC(0b1010) |
                      PORT_PCR_ISF(1);  // Interrupt on falling edge
    GPIOC->PDDR &= ~(1 << 12);
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);  // Enable PORTC interrupt in NVIC
}

volatile int hit = 0;
volatile int misses = 0;
volatile bool stop_loop = false;


// Interrupt Service Routine for PORTC and PORTD
void PORTC_PORTD_IRQHandler(void) {
    if (PORTC->PCR[SW1_PIN] & PORT_PCR_ISF_MASK) {  // SW1 pressed (Right)
        hit++;  // Increment hit counter
        stop_loop = true;
        PORTC->PCR[SW1_PIN] |= PORT_PCR_ISF_MASK;
    }
    if (PORTC->PCR[SW3_PIN] & PORT_PCR_ISF_MASK) {  // SW3 pressed (Left)
        misses++;  // Increment misses counter
        stop_loop = true;
        PORTC->PCR[SW3_PIN] |= PORT_PCR_ISF_MASK;
    }
}

int main(void) {
    disable_watchdog();
    led_green_init();
    led_red_init();
    switch_init();
    irclk_ini();
    lcd_ini();

    lcd_display_dec(666);
    lcd_display_time(12, 24);

    volatile unsigned int sequence = 0x32B14D98;
    unsigned int index = 0;

    while (index < 32) {
        stop_loop = false;  // Reset the loop control

        if (sequence & (1 << index)) {  // Green LED (Right)
            led_green_on();
            while (!stop_loop) {
                // Wait for an interrupt (handled in ISR)
                lcd_display_time(hit, misses);
            }
            led_green_off();
        } else {  // Red LED (Left)
            led_red_on();
            while (!stop_loop) {
                // Wait for an interrupt (handled in ISR)
                lcd_display_time(hit, misses);
            }
            led_red_off();
        }

        index++;  // Move to the next step in the sequence
    }

    // Flash the final result (hits and misses)
    int i = 20;
    while (i > 0) {
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