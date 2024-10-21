#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"

#define SW1_NUM 3 // Derecha
#define SW3_NUM 12 // Left


// Enable IRCLK (Internal Reference Clock)
void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
}

void disable_watchdog() {
    SIM->COPC = 0;
}

void delay(int delayTime) {
    volatile int i;
    for (i = 0; i < delayTime; i++);
}

void led_green_init() {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] |= PORT_PCR_MUX(1);
    GPIOD->PDDR |= 0x20u;
    GPIOD->PSOR |= 1 << 5;
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

void led_red_on() {
    GPIOE->PCOR |= 1 << 29;
}

void led_red_off() {
    GPIOE->PSOR |= 1 << 29;
}

void switch_init() {
    //Se activa el reloj para el puerto C
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    //Se configura el pin como GPIO
    PORTC->PCR[SW1_NUM] |= PORT_PCR_MUX(1);
    PORTC->PCR[SW3_NUM] |= PORT_PCR_MUX(1);

    //Se activa la resistencia
    PORTC->PCR[SW1_NUM] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC->PCR[SW3_NUM] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    //Interrupciones cuando baje el flanco
    PORTC->PCR[SW1_NUM] |= PORT_PCR_IRQC(0b1010);
    PORTC->PCR[SW3_NUM] |= PORT_PCR_IRQC(0b1010);

    //Activar Interrupciones
    NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
    NVIC_SetPriority(PORTC_PORTD_IRQn, 1);
}

volatile int hit = 0;
volatile int misses = 0;
volatile bool stop_loop = false;
volatile bool sw1_pressed = false, sw3_pressed = false;


// Rutina de interrupciones para el puerto D
void PORTDIntHandler(void) {
    if (PORTC->PCR[SW1_NUM] & PORT_PCR_ISF_MASK) {  // SW1 pressed (Right)
        stop_loop = true;
        sw1_pressed = true;
        PORTC->PCR[SW1_NUM] |= PORT_PCR_ISF_MASK;
    }
    if (PORTC->PCR[SW3_NUM] & PORT_PCR_ISF_MASK) {  // SW3 pressed (Left)
        stop_loop = true;
        sw3_pressed = true;
        PORTC->PCR[SW3_NUM] |= PORT_PCR_ISF_MASK;
    }
}

int main(void) {
    disable_watchdog();
    led_green_init();
    led_red_init();
    switch_init();
    irclk_ini();
    lcd_ini();

    volatile unsigned int sequence = 0x32B14D98;
    unsigned int index = 0;

    lcd_display_time(0, 0);
    while (index < 32) {
        stop_loop = false;

        if (sequence & (1 << index)) {  // LED verde (Derecha)
            led_green_on();
            while (!stop_loop) {}
            if (sw1_pressed) {
                hit++;
                sw1_pressed = false;
            } else if (sw3_pressed) {
                misses++;
                sw3_pressed = false;
            }

            led_green_off();
        } else {  // LED rojo (Izquierda)
            led_red_on();
            while (!stop_loop) {}
            if (sw1_pressed) {
                misses++;
                sw1_pressed = false;
            } else if (sw3_pressed) {
                hit++;
                sw3_pressed = false;
            }
            led_red_off();
        }
        lcd_display_time(hit, misses);
        index++;
    }


    int i = 20;
    while (i > 0) {
        i--;
        lcd_display_time(hit, misses);
        delay(2000000);
        lcd_display_time(0, 0);
        delay(2000000);
    }

    while (1) {
    }
    return 0;
}