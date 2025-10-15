#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM
extern volatile uint32_t ms_counter; // Desde main.c

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variables globales de estado
static room_state_t current_state = ROOM_IDLE;
static uint32_t last_action_time = 0;

void room_control_app_init(void)
{
    current_state = ROOM_IDLE;
    last_action_time = 0;

    // Inicializar PWM (PA6)
    tim3_ch1_pwm_init(1000);
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);

    // Asegurar LEDs apagados
    clear_gpio(GPIOA, 5);
    tim3_ch1_pwm_set_duty_cycle(0);

    uart_send_string("Room Control inicializado\r\n");
}

void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        set_gpio(GPIOA, 5);                 // LED integrado ON
        tim3_ch1_pwm_set_duty_cycle(100);   // LED PWM al 100%
        uart_send_string("Estado: OCCUPIED\r\n");
    } else {
        current_state = ROOM_IDLE;
        clear_gpio(GPIOA, 5);               // LED integrado OFF
        tim3_ch1_pwm_set_duty_cycle(0);     // LED PWM OFF
        uart_send_string("Estado: IDLE\r\n");
    }

    // Registrar tiempo de acción
    last_action_time = ms_counter;
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            set_gpio(GPIOA, 5);
            current_state = ROOM_OCCUPIED;
            last_action_time = ms_counter;
            uart_send_string("PWM = 100%\r\n");
            break;

        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            clear_gpio(GPIOA, 5);
            current_state = ROOM_IDLE;
            uart_send_string("PWM = 0%\r\n");
            break;

        case '1':   // 10%
            tim3_ch1_pwm_set_duty_cycle(10);
            set_gpio(GPIOA, 5);
            current_state = ROOM_OCCUPIED;
            last_action_time = ms_counter;
            uart_send_string("PWM = 10%\r\n");
            break;

        case '5':   // 50%
            tim3_ch1_pwm_set_duty_cycle(50);
            set_gpio(GPIOA, 5);
            current_state = ROOM_OCCUPIED;
            last_action_time = ms_counter;
            uart_send_string("PWM = 50%\r\n");
            break;

        case '9':   // 90%
            tim3_ch1_pwm_set_duty_cycle(90);
            set_gpio(GPIOA, 5);
            current_state = ROOM_OCCUPIED;
            last_action_time = ms_counter;
            uart_send_string("PWM = 90%\r\n");
            break;

        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            set_gpio(GPIOA, 5);
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("Sala ocupada\r\n");
            last_action_time = ms_counter;
            break;

        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            clear_gpio(GPIOA, 5);
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Sala vacía\r\n");
            break;

        default:
            uart_send(received_char); // Echo
            break;
    }
}



void room_control_update(void)
{
    // Si está ocupado y han pasado 3 s sin acción, volver a IDLE
    if (current_state == ROOM_OCCUPIED &&
        (ms_counter - last_action_time >= LED_TIMEOUT_MS)) {

        current_state = ROOM_IDLE;
        clear_gpio(GPIOA, 5);
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Timeout -> Estado: IDLE\r\n");
    }
}
