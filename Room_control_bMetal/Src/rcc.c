#include "rcc.h"

void rcc_init(void)
{
    RCC->AHB2ENR |= (1 << 0);                      // Habilita reloj GPIOA
    RCC->AHB2ENR |= (1 << 2);                      // Habilita reloj GPIOC
}
void rcc_gpioa_clock_enable(void)
{
    RCC->AHB2ENR |= (1U << 0);     // GPIOA EN
}

void rcc_gpioc_clock_enable(void)
{
    RCC->AHB2ENR |= (1U << 2);     // GPIOC EN
}

void rcc_syscfg_clock_enable(void)
{
    RCC->APB2ENR |= (1U << 0);     // SYSCFG EN (bit 0)
}

void rcc_usart2_clock_enable(void)
{
    RCC->APB1ENR1 |= (1U << 17);   // USART2 EN
}
// rcc.c
void rcc_tim3_clock_enable(void) {
    RCC->APB1ENR1 |= (1U << 1);   // TIM3 clock enable
}



