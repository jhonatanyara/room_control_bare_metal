# Manual de Usuario — Room Control (Bare‑Metal)

**Proyecto:** Room Control – Bare Metal  
**Versión:** final (14/10/2025)  
**Autor:** Jhonatan Yara López — Curso *Estructuras Computacionales* — Universidad Nacional de Colombia, Sede Manizales

---

## 1. ¿Qué es esto?
Un control sencillo de “iluminación” para una sala usando la tarjeta **Nucleo STM32L476RG**.  
Hay un LED que muestra que el programa está vivo (*heartbeat*), un LED principal que se enciende al pulsar el botón por unos segundos y un LED con brillo regulable por **PWM**. También se puede hablar con la tarjeta por **UART**.

---

## 2. Hardware usado
- **Tarjeta:** Nucleo **STM32L476RG** (alimentada por USB del ST‑LINK).  
- **Heartbeat / Bombilla principal:** LED **LD2 (PA5)** de la propia tarjeta.  
- **Bombilla PWM:** LED externo con resistencia de **220 Ω** entre **PA6** y **GND**.  
  - PA6 está conectado al **TIM3_CH1 (AF2)**.  
- **Botón de usuario:** **B1 / PC13** (activo bajo).  
- **UART:** **PA2 = TX**, **PA3 = RX**, **115200 8N1**.

> Si solo tienes el LED de la placa, úsalo como “bombilla principal” (PA5). El LED PWM sí debe ser externo en **PA6**.

---

## 3. Qué puede hacer
### 3.1 Heartbeat
El LED **LD2/PA5** parpadea de forma regular para indicar que el firmware está corriendo.

### 3.2 Botón = Luz por 3 s
Al presionar **B1 (PC13)**:
- Se enciende **LD2/PA5**.
- Se inicia un temporizador; pasado **~3 s** el LED se apaga solo.
- Se envía un mensaje por UART.

### 3.3 UART (consola serie)
Conéctate a **115200 8N1**. Comandos simples:
- **`h` / `H`** → brillo **100%** del LED PWM (PA6).  
- **`l` / `L`** → brillo **0%** (apagado).  
- **`o` / `O`** → forzar estado *ocupado* (enciende la “sala”).  
- **`i` / `I`** → volver a *idle* (apaga).  
Además, el firmware hace *echo* del texto recibido.

### 3.4 Luz con PWM
El LED en **PA6** usa **PWM a 1 kHz**. El *duty* arranca en **50%** y se puede cambiar con los comandos de la sección anterior.

---

## 4. Cómo está armado por dentro
### 4.1 Módulos de software
- **rcc:** relojes y habilitación de periféricos.  
- **gpio:** manejo de pines.  
- **systick:** *tick* de 1 ms.  
- **uart:** transmisión/recepción por USART2.  
- **nvic:** prioridades y habilitación de interrupciones.  
- **tim:** configuración de **TIM3_CH1** para PWM en **PA6**.  
- *(opcional)* **room_control:** lógica de estados y reglas de la “sala”.

### 4.2 Flujo rápido
1) Se inician **RCC, GPIO, SysTick, UART, NVIC y TIM3**.  
2) El *main loop* atiende UART y vigila tiempos.  
3) Interrupciones: **SysTick** (cuenta ms), **EXTI15_10** (botón), **USART2** (RX).  
4) El PWM de **PA6** queda funcionando en segundo plano.

---

## 5. Cómo usarlo
**Conexión**  
1) USB del **ST‑LINK** a tu PC.  
2) Conecta un LED + 220 Ω entre **PA6** y **GND** (bombilla PWM).  
3) Abre un terminal serie a **115200 8N1** (puerto del ST‑LINK).

**Al encender**  
- Verás `Sistema Inicializado!` por la consola.  
- El **heartbeat** empieza a parpadear.

**Interacción**  
- Pulsa **B1** para encender la luz por 3 s.  
- Usa los comandos por UART para ajustar el PWM o cambiar de estado.

---

## 6. Diagramas (resumen)
**Estados**  
```
 IDLE  --(botón o 'o')-->  OCCUPIED
  ^                           |
  |-----(timeout 3 s o 'i')---|
```

**Componentes**  
CPU (Cortex‑M4) ↔ GPIO (PA5, PC13) ↔ TIM3/PA6 ↔ USART2 (PA2/PA3).

---

## 7. Tips
- Si el botón no responde, revisa que **PC13** esté configurado con *pull‑up* y que el **EXTI** 13 esté habilitado.  
- Si no ves el puerto serie, reinstala el driver del **ST‑LINK** o cambia el cable USB.  
- Si el LED PWM no cambia de brillo, confirma el **AF2** en **PA6** y que **TIM3** tenga reloj.
