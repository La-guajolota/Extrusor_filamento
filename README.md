<!-- @format -->

# ⚙️ Extrusora de Filamento para Impresión 3D

¡Bienvenido/a al repositorio de nuestra **Extrusora de Filamento**!  
Este proyecto surge de la necesidad de **reciclar y reutilizar** plástico (PLA, ABS, PETG, etc.) para convertirlo en filamento nuevo, fomentando la sostenibilidad en la impresión 3D.

---

## 📑 Tabla de Contenidos

1. [Descripción General](#descripción-general)
2. [Características Principales](#características-principales)
3. [Flujo de Trabajo](#flujo-de-trabajo)
4. [Retos Técnicos](#retos-técnicos)
5. [Estructura del Repositorio](#estructura-del-repositorio)
6. [Requisitos y Dependencias](#requisitos-y-dependencias)
7. [Instalación y Uso](#instalación-y-uso)
8. [Colaboradores](#colaboradores)
9. [Licencia](#licencia)

---

## Descripción General

La **Extrusora de Filamento** procesa:

- **Reciclaje** de piezas fallidas y sobrantes de PLA, ABS, PETG…
- **Triturado** y filtrado de partículas a tamaño óptimo.
- **Fusión** controlada con termopares K y MAX6675.
- **Extrusión** de filamento de 1.75 mm con control de diámetro en tiempo real.
- **Enbobinado** sincronizado con motores NEMA para tensión constante.

Diseño modular y open-source para que cualquier persona pueda reproducirlo y mejorarlo.

---

## ✨ Características Principales

- **Modularidad**: subsistemas mecánico, electrónico y firmware independientes.
- **Control térmico**: 3 termopares + resistencias de abrazadera.
- **Motores sincronizados**: extrusión vs. bobinado para diámetro estable.
- **Interfaz de usuario**: LCD + encoder rotativo con menú configurables.
- **Documentación completa**: CAD, esquemas, lista de materiales y guía paso a paso.

## 🔄 Flujo de Trabajo: Recicladora de Filamento

1. **Recolección y clasificación**

   1. Separar por tipo de plástico (PLA, ABS, PETG, etc.)
   2. Retirar impurezas (adhesivos, polvo, material extraño)

2. **Triturado del material**

   1. Introducir las piezas en el sistema de trituración
   2. Filtrar partículas demasiado grandes antes de la extrusión

3. **Fusión y extrusión**

   1. Alimentar las partículas al extrusor
   2. Aplicar temperatura y presión para fundir el material
   3. Extruir el filamento con control de diámetro en tiempo real

4. **Enfriamiento y estabilización**

   1. Pasar el filamento por un baño de enfriamiento o ventiladores
   2. Mantener una tensión constante para evitar deformaciones

5. **Embobinado y almacenamiento**
   1. Medir el diámetro final antes de embobinar
   2. Enrollar en un carrete con tensión controlada
   3. Etiquetar y almacenar el filamento reciclado

---

## 🎯 Retos Técnicos

1. **Triturado de material hasta tamaño ideal**

   1. Trituradora suficientemente potente
   2. Cada material cuenta con propiedades mecánicas diferentes
   3. Filtrado de virutas de tamaño apropiado

2. **Extrusión de filamento**

   1. Temperatura adecuada para cada material (diferentes puntos de fusión)
   2. Presión en el extrusor consistente para garantizar uniformidad diametral
   3. Velocidad de extrusión estable para mantener la calidad del filamento

3. **Enfriamiento**

   1. Un enfriamiento inadecuado hará el material frágil

4. **Embobinado**
   1. Sin un sistema de guiado, el filamento puede enredarse
   2. Control de diámetro para detectar irregularidades durante el bobinado

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

## 🤝 Colaboradores

Este proyecto fue desarrollado por:

1. @DaSa094 (Daniel Salvador Sanchéz Gutierrez)
2. @La-guajolota (Adrián Silva Palafox)
