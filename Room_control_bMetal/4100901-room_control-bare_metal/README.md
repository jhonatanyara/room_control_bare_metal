## Práctica Introductoria: Procesadores - ARM Cortex-M4 y STM32L476RG

**Universidad Nacional de Colombia - Sede Manizales**  
**Curso:** Estructuras Computacionales (4100901)  
**Módulo:** Procesadores y Arquitectura de Conjunto de Instrucciones (ISA)

### 1. Introducción

En este módulo exploraremos los fundamentos de la arquitectura ARM Cortex‑M4 y los principios de los Conjuntos de Instrucciones (ISA) a través de ejemplos prácticos en la placa NUCLEO‑L476RG.

**Objetivos de la práctica:**
- Familiarizarse con la configuración de un proyecto STM32 vacío en VS Code usando CMake.  
- Aprender a incorporar y compilar código ensamblador (ASM) junto con C.  
- Comprender el flujo de ejecución de instrucciones en un procesador Cortex‑M4.  
- Desarrollar diagramas de contexto y componentes para visualizar la interacción entre hardware y código.

### 2. Hardware y Herramientas

- **Placa:** NUCLEO‑L476RG (ARM Cortex‑M4 a 4 MHz)  
- **Editor/IDE:** Visual Studio Code (con extensiones STM32 y CMake Tools)  
- **Compilación:** CMake + toolchain ARM (arm-none-eabi-gcc)  

### 3. Estructura de la Guía

La guía se organiza en varios documentos Markdown que encontrarás en la carpeta `Doc/`:

1. **[SETUP.md](Doc/1_SETUP.md):** Configuración del entorno y creación del proyecto vacío en VS Code; Renombrar `main`, modificar `CMakeLists.txt`.
2. **[ASM_CONFIG.md](Doc/2_ASM_CONFIG.md):** Guía de introducción al set the instrucciones de un procesador (ISA) y al lenguaje ensamblador.
3. **[WORKSHOP_ASM.md](Doc/3_WORKSHOP_ASM.md):** Taller práctico de ensamblador e instrucciones básicas del ISA de ARM.
3. **[WORKSHOP_C.md](Doc/4_WORKSHOP_C.md):** Taller práctico traduciendo las instrucciones básicas del ISA de ARM a `lenguaje C`.

> **Nota:** Cada documento incluye enlaces relativos para facilitar la navegación y referencias cruzadas.

Dirígete primero a [SETUP.md](Doc/1_SETUP.md) para configurar tu entorno de desarrollo.

