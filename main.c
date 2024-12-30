#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"

#define LIGHT_SENSOR_ADC_CHANNEL  (8U)

#define SW1_NUM 3 // Derecha
#define SW3_NUM 12 // Izquierda


void disable_watchdog() {
    SIM->COPC = 0;
}

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
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

void flash_leds() {
    bool static ledsOn = false;
    if (ledsOn) {
        led_green_off();
        led_red_off();
        ledsOn = false;
    } else {
        led_green_on();
        led_red_on();
        ledsOn = true;
    }
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


void ADC_Init(void) {
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    ADC0->CFG1 = ADC_CFG1_ADICLK(0) |
                 ADC_CFG1_MODE(1) |
                 ADC_CFG1_ADIV(2);

    ADC0->SC1[0] = ADC_SC1_ADCH(31);
}

uint16_t ADC_Read(uint8_t channel) {
    ADC0->SC1[0] = channel;
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0];
}


int main(void) {
    uint16_t light_value;
    int light_reference;
    disable_watchdog();
    irclk_ini();
    lcd_ini();
    lcd_display_time(0, 0);

    ADC_Init();
    light_reference = ADC_Read(3);

    while (1) {
        light_value = ADC_Read(3);
        lcd_display_dec(light_value);


        for (volatile int i = 0; i < 1000000; i++);
    }
}