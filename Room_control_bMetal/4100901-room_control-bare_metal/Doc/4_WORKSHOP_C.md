# WORKSHOP_C.md
> **Referencia:**
> Zhu, Yifen. *Embedded Systems with ARM Cortex-M Microcontrollers in Assembly Language and C*, 2nd Edition, 2022. ***Capítulos 7 y 14 recomendados para esta sección***

## 1. Objetivo

Este documento guía la traducción del taller de ensamblador a **C puro**, definiendo todos los registros con macros de dirección y mostrando cómo estructurar el código, explicar su lógica y organizar las funciones.

---

## 2. Preparación del Proyecto

Antes de escribir código, realiza estos pasos:

1. **Crear el archivo de código**: dentro de la carpeta `Src/`, crea un nuevo archivo llamado `workshop.c`.
2. **Incluir en la compilación**: abre `cmake/vscode_generated.cmake` y agrega `Src/workshop.c` a la lista de fuentes del ejecutable. Ejemplo:
   ```cmake
   # Sources
    set(sources_SRCS ${sources_SRCS}
        
        # ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.s
        # ${CMAKE_CURRENT_SOURCE_DIR}/Src/workshop.s
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/workshop.c # added
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/syscall.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32l476rgtx.s
    )
   ```
3. **Agregar el código de ejemplo**: copia el bloque de código de la sección siguiente y pégalo en `Src/workshop.c`.

---

**Nota:**
Este taller debe implementarse en el archivo `Src/workshop.c`.

---

## 3.1. Estructura del archivo C

El archivo `Src/workshop.c` sigue esta estructura:

1. **Includes y Cabeceras**: Incluir `<stdint.h>` y el header propio con defines de registros.  
2. **Defines de Registros**: Macros que mapean periferales a punteros `volatile uint32_t`.  
3. **main()**: Llama a las inicializaciones y entra en el bucle principal.
4. **Funciones de Inicialización**: `init_led()` y `init_systick()`.  
5. **ISR**: `SysTick_Handler()`.  

---

## 3.2. Código de ejemplo (`Src/workshop.c`)

```c
// --- Ejemplo de parpadeo de LED LD2 en STM32L476RGTx -------------------------
#include <stdint.h>


// --- Definiciones de registros para LD2 (Ver RM0351) -------------------------
#define RCC_BASE    0x40021000U
#define RCC_AHB2ENR (*(volatile uint32_t *)(RCC_BASE  + 0x4CU)) // Habilita GPIOA clock

#define GPIOA_BASE  0x48000000U
#define GPIOA_MODER (*(volatile uint32_t *)(GPIOA_BASE + 0x00U)) // Configuración de modo
#define GPIOA_ODR   (*(volatile uint32_t *)(GPIOA_BASE + 0x14U)) // Data de salida
#define LD2_PIN     5U                                         // Pin PA5 (LED)

// --- Definiciones de registros para SysTick (Ver PM0214)  --------------------
#define SYST_BASE   0xE000E010U                                 // SysTick base
#define SYST_CSR    (*(volatile uint32_t *)(SYST_BASE + 0x00U)) // Control y estado
#define SYST_RVR    (*(volatile uint32_t *)(SYST_BASE + 0x04U)) // Valor de recarga
#define HSI_FREQ    4000000U                                   // Reloj interno 4 MHz

#define WFI()       __asm volatile("wfi")

void init_systick(void);
void init_led(void);

// --- Programa principal ------------------------------------------------------
int main(void)
{
    init_led();
    init_systick();
    while (1) {
        WFI();  // Espera interrupción
    }
}

// --- Inicialización de GPIOA PA5 para el LED LD2 -----------------------------
void init_led(void)
{
    RCC_AHB2ENR |= (1 << 0);                      // Habilita reloj GPIOA
    GPIOA_MODER &= ~(3 << (LD2_PIN*2));           // Limpia bits
    GPIOA_MODER |=  (1 << (LD2_PIN*2));           // Configura como salida
}

// --- Inicialización de Systick para 1 s --------------------------------------
void init_systick(void)
{
    SYST_RVR = HSI_FREQ - 1;                      // Recarga = 4000000 - 1
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);    // ENABLE|TICKINT|CLKSOURCE
}

// --- Manejador de la interrupción SysTick ------------------------------------
void SysTick_Handler(void)
{
    GPIOA_ODR ^= (1 << LD2_PIN);                  // Alterna el estado del LED
}
```  

---

## 3.3. Explicación de funciones

| Función            | Descripción                                                                                  |
|--------------------|----------------------------------------------------------------------------------------------|
| `init_led()`       | Habilita reloj a GPIOA y configura PA5 como salida push-pull para controlar el LED.         |
| `init_systick()`   | Programa SysTick para generar una interrupción cada 1 s usando HSI a 4 MHz.                  |
| `SysTick_Handler`  | Interrupción que alterna el bit PA5 en `GPIOA_ODR`, parpadeando el LED.                       |
| `main()`           | Llama a las inicializaciones y entra en bucle WFI para esperar interrupciones.               |

---

## 4. Ejercicio (igual que en ASM)

### Botón y LED en C puro

1. Define macros para **GPIOC_BASE** y habilita GPIOC.  
2. Implementa `init_button()` para configurar **PC13** como entrada.  
3. Sustituye el bucle `while (1)` por uno que:
   - Lee `GPIOC_BASE + 0x10U` (IDR) y detecta nivel bajo en bit 13.  
   - Al pulsar, enciende PA5, espera ~3 s (bucle de NOP con HSI_FREQ), y apaga PA5.

**Siguiente guía:**
Modularización y librerías: [REFACTOR_TO_LIB.md](5_REFACTOR_TO_LIB.md)
