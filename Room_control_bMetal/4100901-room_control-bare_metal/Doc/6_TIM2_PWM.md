# TIM2_PWM.md
> **Referencia:**
> Zhu, Yifen. *Embedded Systems with ARM Cortex-M Microcontrollers in Assembly Language and C*, 2nd Edition, 2022. ***Capítulo 15 recomendado para esta sección***

## 1. Objetivo

Este documento guía la configuración de **TIM2\_CH1** como salida PWM en **C puro**, empleando `typedef struct` para mapear registros de periféricos, definiendo macros para acceso a hardware y organizando las funciones.

---

## 2. Preparación del Proyecto

1. **Crear el archivo de código**: en `Src/`, crea un archivo llamado `tim.c`.
2. **Incluir en la compilación**: agrega `Src/tim.c` en `cmake/vscode_generated.cmake` en la sección de fuentes.
3. **Agregar el código de ejemplo**: copia el bloque de la sección 3 y 4, y pégalo en `Src/tim.c`.

---

## 3. Tipos y Defines

Definimos estructuras que reflejan los registros de RCC, GPIOA, y TIM2:

```c
#include <stdint.h>

// GPIO
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
} GPIO_TypeDef;
#define GPIOA ((GPIO_TypeDef *)0x48000000U)

// RCC
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t ICSCR;
    volatile uint32_t CFGR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t PLLSAI1CFGR;
    volatile uint32_t PLLSAI2CFGR;
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    uint32_t RESERVED1;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t RESERVED2;
    volatile uint32_t APB1RSTR1;
    volatile uint32_t APB1RSTR2;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED3;
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t RESERVED4;
    volatile uint32_t APB1ENR1;
    volatile uint32_t APB1ENR2;
    volatile uint32_t APB2ENR;
} RCC_TypeDef;
#define RCC ((RCC_TypeDef *)0x40021000U)

// TIM2
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RESERVED1;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t RESERVED2;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

#define TIM2 ((TIM_TypeDef *)0x40000000U)


// Constantes
#define TIM2_CLK_FREQ   4000000U   // 4 MHz APB1 timer clock
#define PWM_FREQUENCY   1000U      // 1 kHz PWM
#define PWM_PERIOD      (TIM2_CLK_FREQ / PWM_FREQUENCY)
#define PWM_DUTY_CYCLE  50         // 50% duty cycle
#define PWM_PIN         5U         // PA5 = TIM2_CH1

#define PWM_DC_TO_CCR(DC) ((PWM_PERIOD * (DC)) / 100) // Macro para calcular CCR
```

---

## 4. Código de ejemplo (`Src/workshop_pwm.c`)

```c
// Inicializa PA5 para TIM2_CH1 (AF1)
void init_gpio_pwm(void)
{
    RCC->AHB2ENR |= (1 << 0);                   // Enable GPIOA
    // PA5 alternate function mode
    GPIOA->MODER &= ~(3U << (PWM_PIN * 2));
    GPIOA->MODER |=  (2U << (PWM_PIN * 2));
    // AF1 on PA5 to TIM2_CH1
    GPIOA->AFRL  &= ~(0xFU << (PWM_PIN * 4));
    GPIOA->AFRL  |=  (1U << (PWM_PIN * 4));
}

// Inicializa TIM2 para PWM en CH1
void init_tim2_pwm(void)
{
    RCC->APB1ENR1 |= (1 << 0);                  // Enable TIM2 clock

    TIM2->PSC   = 0;                            // No prescaler
    TIM2->ARR   = PWM_PERIOD - 1;               // Period for 1 kHz

    TIM2->CCMR1 = (6U << 4);                    // PWM mode 1 on CH1
    TIM2->CCER  |= (1 << 0);                    // Enable CH1 output
    TIM2->CCR1  = PWM_DC_TO_CCR(PWM_DUTY_CYCLE);// Set duty cycle
    TIM2->CR1   |= (1 << 0);                    // Enable counter
}

int main(void)
{
    init_gpio_pwm();
    init_tim2_pwm();
    while (1) {
        
    }
}
```

---

## 5. Explicación de funciones

| Función           | Descripción                                                          |
| ----------------- | -------------------------------------------------------------------- |
| `init_gpio_pwm()` | Configura PA6 como AF1 para TIM2\_CH1.                               |
| `init_tim2_pwm()` | Habilita TIM2, programa PSC, ARR, modo PWM1, habilita salida y CCR1. |
| `main()`          | Inicializa GPIO y TIM2 y entra en bucle infinito.                        |

---

## 6. Ejercicio

1. Ajusta **duty cycle** cambiando `TIM2->CCR1` al 10%, 25%, y 75%.
2. Implementa función `set_pwm_duty(uint8_t percent)` que actualice CCR1 en porcentaje.
3. Crea bucle en `main()` que varíe el duty ciclo del 0% al 100% de forma suave.

---

**Siguiente guía:**
UART: [USART2.md](6_USART2.md)
