<!-- @format -->

# ⚙️ Extrusora de Filamento para Impresión 3D

Este proyecto ofrece un diseño abierto para **reciclar** y **reutilizar** residuos plásticos (PLA, ABS, PETG…) convirtiéndolos en filamento de 1.75 mm de alta calidad, fomentando la sostenibilidad en impresión 3D.

---

## 📑 Contenidos

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

---

## 🔄 Flujo de Trabajo

1. **Recolección y clasificación**

   - Separar por tipo de plástico (PLA, ABS, PETG…)
   - Eliminar adhesivos, polvo y contaminantes

2. **Triturado**

   - Triturar al tamaño adecuado
   - Filtrar fragmentos excesivamente grandes

3. **Fusión y extrusión**

   - Alimentar extrusor
   - Aplicar temperatura y presión
   - Control de diámetro en tiempo real

4. **Enfriamiento**

   - Baño o flujo de aire
   - Tensión constante para evitar deformaciones

5. **Embobinado y almacenamiento**
   - Medir diámetro final
   - Enrollar con tensión controlada
   - Etiquetar y almacenar

---

## 🎯 Retos Técnicos

1. **Triturado**

   - Potencia y geometría de cuchillas adecuadas
   - Filtrado de partículas homogéneas

2. **Extrusión**

   - Temperaturas variables según material
   - Presión y velocidad constantes para diámetro uniforme

3. **Enfriamiento**

   - Evitar fragilidad por enfriado demasiado rápido

4. **Embobinado**
   - Sistema guiado para prevenir enredos
   - Monitoreo de diámetro durante el bobinado

---

## 📂 Estructura del Repositorio

```text
.
├── Electrónica/               # Esquemas, PCB y conexiones
├── Mecánica/                  # Modelos CAD y planos
├── Firmware/                  # Código STM32CubeIDE
├── Investigación_fuentes/     # Artículos y referencias
├── Presupuestos_materiales/   # Lista de componentes y costes
├── LICENSE                    # Licencia MIT
└── README.md                  # Documentación principal
```
