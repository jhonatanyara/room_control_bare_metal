#include "gpio.h"
#include "systick.h"
#include "rcc.h"
#include "uart.h"
#include "nvic.h"
#include "tim.h"
#include "room_control.h"

// Flags para eventos
volatile uint8_t button_event = 0;
volatile char uart_event_char = 0;

// Contador de milisegundos del sistema
volatile uint32_t ms_counter = 0;

// Función local para inicializar periféricos
static void peripherals_init(void)
{
    // Inicialización del sistema
    rcc_init();

    // Configuración de GPIOs
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00);

    // Inicialización de periféricos
    init_systick(); // 1ms con HSI a 4MHz
    init_uart();   // USART2 a 9600 baudios
    nvic_exti_pc13_button_enable();
    nvic_usart2_irq_enable();
    tim3_ch1_pwm_init(1000);  // 1 kHz PWM
}

int main(void)
{
    peripherals_init();
    room_control_app_init();
    uart_send_string("Sistema de Control de Sala Inicializado!\r\n");

    // Bucle principal: procesa eventos
    while (1) {
        if (button_event) {
            button_event = 0;
            room_control_on_button_press();
        }
        if (uart_event_char) {
            char c = uart_event_char;
            uart_event_char = 0;
            room_control_on_uart_receive(c);
        }
        // Llamar a la función de actualización periódica
        room_control_update();
    }
}

// Manejador de SysTick
void SysTick_Handler(void)
{
    ms_counter++;
}

// Manejadores de interrupciones
void EXTI15_10_IRQHandler(void)
{
    // Limpiar flag de interrupción
    if (EXTI->PR1 & (1 << 13)) {
        EXTI->PR1 |= (1 << 13);  // Clear pending
        button_event = 1;
    }
}

void USART2_IRQHandler(void)
{
    // Verificar si es recepción
    if (USART2->ISR & (1 << 5)) {  // RXNE
        uart_event_char = (char)(USART2->RDR & 0xFF);
    }
}