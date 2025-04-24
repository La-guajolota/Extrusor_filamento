<!-- @format -->

# ⚙️ Extrusora de Filamento para Impresión 3D

¡Bienvenido/a al repositorio de nuestra **Extrusora de Filamento**!  
Este proyecto surge de la necesidad de **reciclar y reutilizar** plástico (PLA, ABS, PETG, etc.) para convertirlo en filamento nuevo, fomentando la sostenibilidad en la impresión 3D.

---

## 📑 Tabla de Contenidos

1. [Descripción General](#descripción-general)
2. [Características Principales](#características-principales)
3. [Estructura del Repositorio](#estructura-del-repositorio)
4. [Requisitos y Dependencias](#requisitos-y-dependencias)
5. [Instalación y Uso](#instalación-y-uso)
6. [Colaboradores](#colaboradores)
7. [Licencia](#licencia)

---

## Descripción General

Con esta **Extrusora de Filamento** podrás:

- **Reciclar** piezas fallidas, soportes y purgas de PLA, ABS, PETG, etc.
- **Fundir** el plástico de forma controlada mediante termopares tipo K y controladores MAX6675.
- **Extruir** filamento de 1.75 mm con diámetro uniforme.
- **Enbobinar** automáticamente el filamento con sincronización de dos motores NEMA.

El objetivo es ofrecer un diseño **abierto y reproducible**, que cualquiera pueda ensamblar, usar y mejorar.

---

## ✨ Características Principales

1. **Arquitectura modular**
   - Mecánica, electrónica y firmware separados para facilitar mantenimiento y evolución.
2. **Control de temperatura**
   - Tres termopares tipo K controlados por MAX6675 y resistencias de abrazadera en el cilindro.
3. **Sincronización de motores**
   - Un NEMA acciona el tornillo de extrusión y otro gestiona el carrete para un filamento constante.
4. **Interfaz intuitiva**
   - Pantalla LCD y encoder rotativo con menú para ajustar temperatura, velocidad y tiempos.
5. **Documentación completa**
   - Planos CAD, esquemas eléctricos, lista de materiales y guía paso a paso para montaje y calibración.

---

## 📂 Estructura del Repositorio

```bash
.
├── Electrónica/               # Esquemas, PCB y conexiones
├── Mecánica/                  # Modelos CAD y planos
├── Firmware/                  # Código embebido (STM32CubeIDE)
├── Investigación_fuentes/     # Artículos y referencias
├── Presupuestos_materiales/   # Listado de componentes y costes
├── LICENSE                    # Licencia de código abierto
└── README.md                  # Documentación principal
```

## Colaboradores

Este proyecto fue desarrollado por:

1. @DaSa094 (Daniel Salvador Sanchéz Gutierrez)
2. @La-guajolota (Adrián Silva Palafox)
