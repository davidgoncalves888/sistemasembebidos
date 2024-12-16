#include <stdlib.h>
#include "MKL46Z4.h"
#include "lcd.h"

#define SW1_NUM 3 // Derecha
#define SW3_NUM 12 // Izquierda
volatile int ss = 0;
volatile int aa = 0;

void disable_watchdog() {
    SIM->COPC = 0;
}

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
}

void updateLCD() {
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
    // Step 1: Enable MCGIRCLK (32.768 kHz)
    MCG->C1 |= MCG_C1_IRCLKEN_MASK;  // Enable MCGIRCLK
    MCG->C2 &= ~MCG_C2_IRCS_MASK;    // Select slow mode (32.768 kHz)

    // Step 2: Select MCGIRCLK as TPM clock source
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);

    // Step 3: Enable TPM0 clock
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // Step 4: Configure TPM0 for 1-second interval
    TPM0->SC = TPM_SC_PS(5);         // Set prescaler to 32
    TPM0->MOD = 1024 - 1;            // Set MOD for 1-second interval
    TPM0->SC |= TPM_SC_CMOD(1);      // Start TPM

    // Step 5: Enable overflow interrupt
    TPM0->SC |= TPM_SC_TOIE_MASK;    // Enable TOF interrupt
    NVIC_EnableIRQ(TPM0_IRQn);       // Enable NVIC for TPM0
}

// TPM0 Interrupt Handler
void FTM0IntHandler(void) {
    if (TPM0->STATUS & TPM_STATUS_TOF_MASK) {
        TPM0->STATUS |= TPM_STATUS_TOF_MASK;  // Clear TOF
        ss--;                            // Increment seconds counter
        lcd_display_time(ss, aa);

    }
}

void PORTDIntHandler(void) {
    if (PORTC->PCR[SW1_NUM] & PORT_PCR_ISF_MASK) {  // SW1 pressed (Right)
        ss++;
        lcd_display_time(ss, aa);
        PORTC->PCR[SW1_NUM] |= PORT_PCR_ISF_MASK;
    }
    if (PORTC->PCR[SW3_NUM] & PORT_PCR_ISF_MASK) {  // SW3 pressed (Left)
        TPM0_Init();
        PORTC->PCR[SW3_NUM] |= PORT_PCR_ISF_MASK;
    }
}

int main() {
    disable_watchdog();
    irclk_ini();
    lcd_ini();
    switch_init();
    lcd_display_time(ss, aa);

    while (1) {
        // Keep the program running
    }

    return 0;
}