# Refactorización: Uso de Librerías para Periféricos

# Refactorización: Uso de Librerías para Periféricos

> **Referencia:**
> Zhu, Yifen. *Embedded Systems with ARM Cortex-M Microcontrollers in Assembly Language and C*, 2nd Edition, 2022. ***Capítulo 8 recomendado para esta sección***

En esta guía aprenderás a modularizar tu proyecto STM32 usando librerías para los periféricos principales. Esto mejora la organización y reutilización del código.

## 1. Estructura Recomendada

```
inc/
  gpio.h
  systick.h
  rcc.h

src/
  main.c
  gpio.c
  systick.c
  rcc.c
```

## 2. Ejemplo de Uso en `main.c`

```c
#include "gpio.h"
#include "systick.h"
#include "rcc.h"

int main(void) {
    rcc_init();
    gpio_init();
    systick_init();
    while (1) {
        gpio_toggle(GPIO_PIN_5);
        systick_delay_ms(500);
    }
    return 0;
}
```

## 3. Implementación de Librerías

### gpio.c/h
- Inicializa y controla pines digitales.
- Funciones: `gpio_init()`, `gpio_toggle(pin)`

### systick.c/h
- Configura el temporizador SysTick para retardos.
- Funciones: `systick_init()`, `systick_delay_ms(ms)`

### rcc.c/h
- Configura los relojes del sistema.
- Función: `rcc_init()`

## 4. Siguientes pasos

- Refactoriza el código del taller C para usar estas librerías.
- Implementa nuevas librerías para otros periféricos (PWM, USART) siguiendo este patrón.
- Consulta las guías avanzadas para PWM y USART al final del curso.

---

> Esta estructura facilita la comprensión y el mantenimiento del código en proyectos embebidos.

**Siguiente guía:**
Timers y PWM: [TIM2_PWM.md](6_TIM2_PWM.md)
