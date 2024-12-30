#include "MKL46Z4.h"
#include "lcd.h"

void disable_watchdog() {
    SIM->COPC = 0;
}

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
}

void ADC_Init(void) {
    // Enable the clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configure ADC0
    ADC0->CFG1 = ADC_CFG1_ADICLK(0) | // Bus clock source
                 ADC_CFG1_MODE(1) |   // 12-bit conversion
                 ADC_CFG1_ADIV(2);    // Divide input clock by 4

    ADC0->SC1[0] = ADC_SC1_ADCH(31);  // Disable module (default state)
}

uint16_t ADC_Read(uint8_t channel) {
    // Selección de canal
    ADC0->SC1[0] = channel;
    // Esperar hasta que acabe la conversión
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0];
}


void PWM_Init(void) {
    // Reloj
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // Configurar puertos
    PORTD->PCR[5] = PORT_PCR_MUX(4);
    PORTE->PCR[29] = PORT_PCR_MUX(3);

    // Fuente reloj
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    // TPM0 a PWM
    TPM0->SC = TPM_SC_CMOD(1) | TPM_SC_PS(4);
    TPM0->MOD = 255;

    // TPM0_CH5 para el LED verde
    TPM0->CONTROLS[5].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0->CONTROLS[5].CnV = 0;

    // TPM0_CH2 para el LED rojo
    TPM0->CONTROLS[2].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0->CONTROLS[2].CnV = 0;
}

void PWM_SetDutyCycle(TPM_Type *tpm, uint8_t channel, uint8_t duty_cycle) {
    if (duty_cycle > 100) duty_cycle = 100;
    tpm->CONTROLS[channel].CnV = (tpm->MOD * duty_cycle) / 100;
}


int main(void) {
    uint16_t light_value;
    uint8_t green_duty_cycle, red_duty_cycle;
    disable_watchdog();
    irclk_ini();
    lcd_ini();
    lcd_display_time(0, 0);
    ADC_Init();
    PWM_Init();

    while (1) {
        light_value = ADC_Read(3);
        lcd_display_dec(light_value);
        green_duty_cycle = 100 - (light_value * 100 / 4095);
        red_duty_cycle = 100 - (light_value * 100 / 4095);
        PWM_SetDutyCycle(TPM0, 5, green_duty_cycle);
        PWM_SetDutyCycle(TPM0, 2, red_duty_cycle);
        for (volatile int i = 0; i < 1000000; i++);
    }
}