#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"

#define SW1_NUM 3 // Derecha
#define SW3_NUM 12 // Izquierda

volatile int ss = 0;
volatile int aa = 0;
volatile bool inputingSs = true; //Controla si se está escribiendo ss o aa
volatile bool counting = false; //Controla si ya se ha acabado de meter ss y aa
volatile bool pauseCounting = false; //Controla si el usuario ha pausado la cuenta

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

void TPM0_Init(void) {
    // Activar MCGIRCLK a (32.768 kHz)
    MCG->C1 |= MCG_C1_IRCLKEN_MASK;  // Activar MCGIRCLK
    MCG->C2 &= ~MCG_C2_IRCS_MASK;    // Modo (32.768 kHz)

    // MCGIRCLK como reloj
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);

    // Activar TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // TPM0 a 1 s
    TPM0->SC = TPM_SC_PS(5);         // Escalar a 32
    TPM0->MOD = 1024 - 1;            // Módulo para 1 s
    TPM0->SC |= TPM_SC_CMOD(1);      // Inicial contador

    // Activar interrupciones
    TPM0->SC |= TPM_SC_TOIE_MASK;
    NVIC_EnableIRQ(TPM0_IRQn);
}

void FTM0IntHandler(void) {
    if (TPM0->STATUS & TPM_STATUS_TOF_MASK) {
        TPM0->STATUS |= TPM_STATUS_TOF_MASK;
        if (ss > 0 && !pauseCounting) {
            ss--;
        }
        if (ss <= aa) {
            flash_leds();
        }
        lcd_display_time(ss, aa);

    }
}

void PORTDIntHandler(void) {
    if (PORTC->PCR[SW1_NUM] & PORT_PCR_ISF_MASK) {  // SW1 pressed (Right)
        if (inputingSs) {
            ss++;
        } else {
            aa++;
        }
        lcd_display_time(ss, aa);
        PORTC->PCR[SW1_NUM] |= PORT_PCR_ISF_MASK;
    }
    if (PORTC->PCR[SW3_NUM] & PORT_PCR_ISF_MASK) {  // SW3 pressed (Left)
        if (inputingSs && !counting) {
            inputingSs = false;
        } else {
            inputingSs = true;
            counting = true;
            TPM0_Init();
        }

        if (counting) {
            pauseCounting = !pauseCounting;
        }
        PORTC->PCR[SW3_NUM] |= PORT_PCR_ISF_MASK;
    }
}

int main() {
    disable_watchdog();
    irclk_ini();
    lcd_ini();
    switch_init();
    led_green_init();
    led_red_init();
    lcd_display_time(ss, aa);

    while (1) {
    }

    return 0;
}