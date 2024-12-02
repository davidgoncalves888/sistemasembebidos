#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "MKL46Z4.h"
#include "lcd.h"

#define QUEUE_LENGTH 99
#define QUEUE_ITEM_SIZE sizeof(int)
#define SW1_NUM 3 // Derecha
#define SW3_NUM 12 // Izquierda

QueueHandle_t messageQueue;
volatile int numProducers = 0;
volatile int numConsumers = 0;

void disable_watchdog() {
    SIM->COPC = 0;
}

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock; 1 = 4MHz irc
}

void updateLCD(int pendingMessages) {
    lcd_display_time(numProducers * 10 + numConsumers, pendingMessages);
}

// Productor de datos
void producerTask(void *pvParameters) {
    int data;
    while (1) {
        if (numProducers > 0) {
            data = rand() % 100; // Generar datos aleatorios
            if (xQueueSend(messageQueue, &data, portMAX_DELAY) == pdPASS) {
                updateLCD(uxQueueMessagesWaiting(messageQueue));
            }
            vTaskDelay(pdMS_TO_TICKS(1000 / numProducers)); // Delay en función de la cantidad de productores
        }
    }
}

// Consumidor de datos
void consumerTask(void *pvParameters) {
    int data;
    while (1) {
        if (numConsumers > 0) {
            if (xQueueReceive(messageQueue, &data, portMAX_DELAY) == pdPASS) {
                updateLCD(uxQueueMessagesWaiting(messageQueue));
            }
            vTaskDelay(pdMS_TO_TICKS(1000 / numConsumers)); // Delay en función de la cantidad de consumidores
        }
    }
}

void adjustProducersConsumers() {
    numProducers = 4;
    numConsumers = 1;
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

void PORTDIntHandler(void) {
    if (PORTC->PCR[SW1_NUM] & PORT_PCR_ISF_MASK) {  // SW1 pressed (Right)
        numConsumers >= 4 ? numConsumers = 0 : numConsumers++;
        PORTC->PCR[SW1_NUM] |= PORT_PCR_ISF_MASK;
    }
    if (PORTC->PCR[SW3_NUM] & PORT_PCR_ISF_MASK) {  // SW3 pressed (Left)
        numProducers >= 4 ? numProducers = 0 : numProducers++;
        PORTC->PCR[SW3_NUM] |= PORT_PCR_ISF_MASK;
    }
}

int main() {
    disable_watchdog();
    irclk_ini();
    lcd_ini();
    switch_init();
    lcd_display_time(0, 0);
    messageQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    if (messageQueue == NULL) {
        lcd_display_time(1, 1);
        return 1;
    }

    // Crear tareas de productor y consumidor
    xTaskCreate(producerTask, "ProducerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(consumerTask, "ConsumerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Simular cambio en productores y consumidores
    adjustProducersConsumers();

    // Iniciar el scheduler
    vTaskStartScheduler();

    // Este punto no debería alcanzarse
    for (;;);
    return 0;
}