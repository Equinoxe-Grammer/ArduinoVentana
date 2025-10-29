# Alarma de Ventana con Sensor IR (KY-033) y Buzzer – Arduino Nano

## ¿Qué hace el proyecto?
Implementa una **alarma antintrusión** para ventanas usando un sensor IR de línea (KY-033) y un buzzer. Detecta:
- **Ventana cerrada (NEGRO)** → buzzer **apagado**.
- **Apertura/movimiento (BLANCO)** → buzzer **intermitente** (50/50 ms).
- **Manipulación/Golpe (tamper)** → **alarma fuerte** (encendido continuo durante 15 s).

### Problema que resuelve
Los sensores de vibración pueden no dispararse si abren sin golpear y provocar falsos positivos por vibraciones del entorno. El IR detecta **apertura real** por contraste **negro/blanco**, con menos falsos positivos y reacción temprana.

### Propósito, contexto y alcance
- **Propósito:** solución educativa y de bajo costo para seguridad doméstica.
- **Contexto:** proyectos escolares, makers, prototipos rápidos.
- **Alcance:** detección por contraste y lógica simple con "tamper"; extensible a reed switch, vibración, PIR.

## ¿Cómo funciona internamente?

### Arquitectura general (Mermaid)
```mermaid
flowchart LR
  IR[Sensor IR (KY-033)] --|OUT (D3)| MCU[Arduino Nano]
  MCU --|D2| BZ[Buzzer (alarma)]
```

### Diagrama de flujo (Mermaid)
```mermaid
flowchart TD
  A[Leer D3] --> B{Cambio de estado?}
  B -- Sí --> C[edgeCount++ y marca tiempo]
  B -- No --> D[¿estable > 80ms?]
  C --> D
  D -- Sí --> E[Actualiza estado estable]
  D -- No --> A
  E --> F{edgeCount >= 5 en 300ms?}
  F -- Sí --> G[Tamper ON 15 s]
  F -- No --> H{Estado estable LOW? (BLANCO)}
  G --> I[Buzzer ON continuo]
  H -- Sí --> J[Parpadeo 50/50 ms]
  H -- No --> K[Buzzer OFF]
  I --> A
  J --> A
  K --> A
```

### Estructura del proyecto
```
.
├── README.md
├── src/alarma_ir.ino
├── docs/diagramas/
├── hardware/
├── tests/
└── lib/
```

### Tecnologías usadas

- **Arduino Nano (ATmega328P)** con CH340 (USB-Serial).
- **Sensor IR KY-033** (salida digital; sensibilidad con potenciómetro).
- **Buzzer 5V** (ideal activo).
- **C++/Arduino** (funciones: `pinMode`, `digitalRead`, `digitalWrite`, `millis()`; opcional `tone()`).

### Comunicación y decisiones técnicas

- **D3** ← salida digital del sensor (HIGH = negro/cerrado; LOW = blanco/abierto).
- **D2** → buzzer. En tamper: ON continuo. En abierto: parpadeo 50/50 ms.
- Lógica no bloqueante con `millis()`, filtro de estabilidad (80 ms) y ventana de tamper (≥5 cambios en 300 ms; latch 15 s).
- Extensible a otros sensores (OR lógico a la misma alarma).

## ¿Cómo se usa o contribuye alguien más?

### Requisitos e instalación

- **Arduino IDE 2.x** (o PlatformIO).
- **Driver CH340** si no aparece el puerto.
- **Conexiones:**
  - Sensor: VCC→5V, GND→GND, OUT→D3
  - Buzzer activo: (+)→D2, (–)→GND (si >20 mA: transistor + diodo)
  - Marca negra = cerrado; blanco = abierto
- Ajusta el potenciómetro del KY-033 para que con negro esté HIGH y con blanco LOW.

### Ejecutar entorno local

1. Abre `src/alarma_ir.ino`.
2. **Herramientas** → **Placa:** Arduino Nano; **Procesador:** ATmega328P (o Old Bootloader si falla).
3. **Puerto:** USB-SERIAL CH340.
4. Compila y sube (Uploading ~2–6 s).

### Pruebas

- **Negro (cerrado)** → silencio.
- **Blanco (abierto)** → pitido 50/50 ms.
- **Varias transiciones rápidas (<300 ms)** → tamper (alarma continua 15 s).

### Contribuir

- PRs con descripción clara y cambios mínimos.
- Mantén estilo, comenta la lógica, actualiza README y diagramas si cambias arquitectura.
- Añade pruebas cuando modifiques la lógica.

## FAQ

**¿No suena el buzzer?** Asegúrate que sea activo 5V y polaridad correcta. Si es pasivo, usa `tone()` o un driver.  
**¿Se dispara solo?** Calibra el potenciómetro del KY-033 y fija distancia/ángulo.  
**¿Falla la carga?** Placa: Nano, Procesador: ATmega328P (o Old Bootloader), cable de datos y puerto CH340.  
**¿Más volumen?** Transistor/MOSFET y sirena externa (9–12V). Nunca directo al pin.

## Resumen técnico

| **Pines**   | D3 (sensor), D2 (buzzer)                           |
|-------------|---------------------------------------------------|
| **Filtros** | estable ≥ 80 ms; tamper: ≥ 5 edges / 300 ms; hold 15 s |
| **Parpadeo**| 50/50 ms (apertura) \| Tamper: ON continuo (fuerte) |
