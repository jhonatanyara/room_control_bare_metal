@ --- Ejemplo de parpadeo de LED LD2 en STM32L476RG ---------------------------
    .section .text
    .syntax unified
    .thumb

    .global main
    .global init_led
    .global init_systick
    .global SysTick_Handler
    .global init_button
    .global poll_button

@ --- Botón B1 (PC13) ---
    .equ GPIOC_BASE,     0x48000800         @ Base de GPIOC
    .equ GPIOC_MODER,    GPIOC_BASE + 0x00  @ Mode register
    .equ GPIOC_IDR,      GPIOC_BASE + 0x10  @ Input data register
    .equ BTN_PIN,        13                 @ PC13

@ --- Definiciones de registros para LD2 (GPIOA/LD2) --------------------------
    .equ RCC_BASE,       0x40021000         @ Base de RCC
    .equ RCC_AHB2ENR,    RCC_BASE + 0x4C    @ Enable GPIOx clock (AHB2ENR)
    .equ GPIOA_BASE,     0x48000000         @ Base de GPIOA
    .equ GPIOA_MODER,    GPIOA_BASE + 0x00  @ Mode register
    .equ GPIOA_ODR,      GPIOA_BASE + 0x14  @ Output data register
    .equ LD2_PIN,        5                  @ Pin del LED LD2 (PA5)

@ --- Definiciones de registros para SysTick (PM0214) -------------------------
    .equ SYST_CSR,       0xE000E010         @ Control and status
    .equ SYST_RVR,       0xE000E014         @ Reload value register
    .equ SYST_CVR,       0xE000E018         @ Current value register

@ --- Reloj del sistema -------------------------------------------------------
    .equ SYSCLK_HZ,      4000000         @ En reset: MSI ~ 4 MHz

@ --- Variable: contador de segundos para el LED ------------------------------
    .section .bss
    .align 4
led_timer:
    .space 4                                 @ (0 = apagado)

    .section .text

@ --- Programa principal ------------------------------------------------------
main:
    bl init_led
    bl init_button
    bl init_systick

loop:
    bl  poll_button                          @ si se pulsa, enciende LED y carga 3 s
    wfi
    b   loop

@ --- Inicialización de GPIOA PA5 para el LED LD2 -----------------------------
init_led:
    @ Habilita reloj de GPIOA (AHB2ENR.GPIOAEN bit0)
    movw  r0, #:lower16:RCC_AHB2ENR
    movt  r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]
    orr   r1, r1, #(1 << 0)
    str   r1, [r0]

    @ PA5 como salida (MODER5 = 0b01)
    movw  r0, #:lower16:GPIOA_MODER
    movt  r0, #:upper16:GPIOA_MODER
    ldr   r1, [r0]
    bic   r1, r1, #(0b11 << (LD2_PIN * 2))
    orr   r1, r1, #(0b01 << (LD2_PIN * 2))
    str   r1, [r0]
    bx    lr

@ --- Inicialización de PC13 como entrada (Botón B1) --------------------------
init_button:
    @ Habilita reloj de GPIOC (AHB2ENR.GPIOCEN bit2)
    movw  r0, #:lower16:RCC_AHB2ENR
    movt  r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]
    orr   r1, r1, #(1 << 2)
    str   r1, [r0]

    @ PC13 en modo entrada (MODER13 = 0b00)
    movw  r0, #:lower16:GPIOC_MODER
    movt  r0, #:upper16:GPIOC_MODER
    ldr   r1, [r0]
    bic   r1, r1, #(0b11 << (BTN_PIN * 2))
    str   r1, [r0]
    bx    lr

@ --- Lectura del botón y armado del temporizador -----------------------------
poll_button:
    @ Lee PC13: en Nucleo, PRESIONADO = 0
    movw  r0, #:lower16:GPIOC_IDR
    movt  r0, #:upper16:GPIOC_IDR
    ldr   r1, [r0]
    tst   r1, #(1 << BTN_PIN)
    bne   poll_exit                          @ si bit=1 -> NO presionado

    @ Si presionado: enciende LED (PA5=1) y carga 3 segundos
    movw  r0, #:lower16:GPIOA_ODR
    movt  r0, #:upper16:GPIOA_ODR
    ldr   r1, [r0]
    orr   r1, r1, #(1 << LD2_PIN)            @ LED ON
    str   r1, [r0]

    @ led_timer = 3
    ldr   r0, =led_timer
    movs  r1, #3
    str   r1, [r0]

poll_exit:
    bx    lr

@ --- Inicialización de SysTick para 1 s --------------------------------------
init_systick:
    @ RVR = SYSCLK_HZ - 1  -> periodo 1 s
    movw  r0, #:lower16:SYST_RVR
    movt  r0, #:upper16:SYST_RVR
    movw  r1, #:lower16:SYSCLK_HZ
    movt  r1, #:upper16:SYSCLK_HZ
    subs  r1, r1, #1
    str   r1, [r0]

    @ Limpia CVR para arrancar y borrar COUNTFLAG
    movw  r0, #:lower16:SYST_CVR
    movt  r0, #:upper16:SYST_CVR
    movs  r1, #0
    str   r1, [r0]

    @ ENABLE=1, TICKINT=1, CLKSOURCE=1 (CPU clock)
    movw  r0, #:lower16:SYST_CSR
    movt  r0, #:upper16:SYST_CSR
    movs  r1, #(1 << 2) | (1 << 1) | (1 << 0)
    str   r1, [r0]
    bx    lr

@ --- Manejador de la interrupción SysTick ------------------------------------
    .thumb_func
SysTick_Handler:
    @ if (led_timer > 0) led_timer--;
    ldr   r0, =led_timer
    ldr   r1, [r0]
    cbz   r1, syst_done                      @ si 0, nada que hacer
    subs  r1, r1, #1
    str   r1, [r0]
    bne   syst_done                          @ si aún >0, salir

    @ Si llegó a 0: apaga LED (PA5=0)
    movw  r0, #:lower16:GPIOA_ODR
    movt  r0, #:upper16:GPIOA_ODR
    ldr   r1, [r0]
    bic   r1, r1, #(1 << LD2_PIN)            @ LED OFF
    str   r1, [r0]

syst_done:
    bx    lr

