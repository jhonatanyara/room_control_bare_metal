# USART2.md
> **Referencia:**
> Zhu, Yifen. *Embedded Systems with ARM Cortex-M Microcontrollers in Assembly Language and C*, 2nd Edition, 2022. ***Capítulo 22 recomendado para esta sección***

## 1. Objetivo

Este documento guía la implementación de comunicación UART en **C puro** usando **typedef struct** para mapear los periféricos, y muestra cómo estructurar el código, explicar su lógica y organizar las funciones.

---

## 2. Preparación del Proyecto

Antes de escribir código, realiza estos pasos:

1. **Crear el archivo de código**: en `Src/`, crea un archivo llamado `uart.c`.
2. **Incluir en la compilación**: agrega `Src/uart.c` en `cmake/vscode_generated.cmake` dentro de la lista de fuentes.
3. **Agregar el código de ejemplo**: copia el bloque de la sección 4 y pégalo en `Src/uart.c`.

---

## 3. Tipos y Defines

Definimos estructuras para mapear los registros de los periféricos y punteros a sus bases:

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

// SysTick
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;
#define SysTick ((SysTick_TypeDef *)0xE000E010U)

// USART2
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
} USART_TypeDef;
#define USART2 ((USART_TypeDef *)0x40004400U)

// NVIC
#define NVIC_ISER1 (*(volatile uint32_t*)0xE000E104U)

// Constantes
#define LD2_PIN        5U
#define BUTTON_PIN    13U
#define BAUD_RATE     115200U
#define HSI_FREQ      4000000U
```

---

## 4. Código de ejemplo (`Src/uart.c`)

```c
void init_gpio_uart(void) {
    // PA2->TX AF7, PA3->RX AF7
    RCC->AHB2ENR |= (1 << 0);  // Enable GPIOA
    GPIOA->MODER &= ~((3U<<4)|(3U<<6));
    GPIOA->MODER |=  ((2U<<4)|(2U<<6));
    GPIOA->AFRL &= ~((0xFU<<8)|(0xFU<<12));
    GPIOA->AFRL |=  ((7U<<8)|(7U<<12));  // AF7 = USART2
}

void init_uart(void) {
    RCC->APB1ENR1 |= (1 << 17);  // Enable USART2
    USART2->BRR = (HSI_FREQ + (BAUD_RATE/2)) / BAUD_RATE;
    USART2->CR1 = (1 << 3) | (1 << 2);  // TE | RE
    USART2->CR1 |= (1 << 0);            // UE
    
    USART2->CR1 |= (1 << 5);            // RXNE interrupt
    NVIC_ISER1 |= (1U << 6);            // IRQ38
}

void uart_send(char c) {
    while (!(USART2->ISR & (1 << 7)));   // TXE
    USART2->TDR = (uint8_t)c;
}

char uart_receive(void) {
    while (!(USART2->ISR & (1 << 5)));   // RXNE
    return (char)(USART2->RDR & 0xFF);
}

void USART2_IRQHandler(void) {
    if (USART2->ISR & (1 << 5)) {
        char c = USART2->RDR;
        USART2->ICR = (1 << 5);          // Clear RXNE flag
        uart_send(c);                    // Echo
    }
}

int main(void) {
    init_gpio_uart();
    init_uart();
    const char *msg = "UART listo!\r\n";
    for (const char *p = msg; *p; ++p) uart_send(*p);
    while (1) {
        __asm volatile("wfi");
    }
}
```

---

## 5. Explicación de funciones

| Función               | Descripción                                                                  |
| --------------------- | ---------------------------------------------------------------------------- |
| `init_gpio_uart()`    | Configura PA2/PA3 como AF7 para USART2.                                      |
| `init_uart()`         | Habilita RCC APB1 USART2, configura BRR, enciende UE/TE/RE y RXNE interrupt. |
| `uart_send(char)`     | Espera TXE y escribe en TDR.                                                 |
| `uart_receive(void)`  | Espera RXNE y lee RDR.                                                       |
| `USART2_IRQHandler()` | ISR que lee RDR, limpia RXNE y hace eco.                                     |
| `main()`              | Inicializa todo, envía mensaje y espera IRQ con WFI.                         |

---

## 6. Ejercicio

1. Cambia la configuración de tasa de baudios en `init_uart()`.
2. Implementa `void uart_send_string(const char*)`.
3. Recibe línea completa fuera de la ISR y envía mensaje: **"Recibido: \<línea>\r\n"**.
4. Documenta pros/cons de polling vs interrupciones.

