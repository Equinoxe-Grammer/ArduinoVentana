/*
  alarma_ir.ino

  Firmware para un sistema de alarma de ventana que utiliza un sensor infrarrojo de seguimiento de línea (KY‑033) y un buzzer.  El código implementa tres modos de detección:

  1. Ventana cerrada (se detecta negro): el buzzer permanece apagado.
  2. Ventana abierta o movimiento lento (se detecta blanco): el buzzer emite un beep intermitente de 50 ms encendido y 50 ms apagado.
  3. Tamper o manipulación (golpes o cambios rápidos): si se detectan al menos 5 cambios de nivel en el sensor en una ventana de 300 ms, se activa una alarma fuerte (buzzer encendido continuamente) durante 15 s.

  Este sketch está diseñado para Arduino Nano (ATmega328P) pero funciona igualmente en placas Uno o Mega.  Ajusta los pines si lo necesitas.  Utiliza técnicas no bloqueantes basadas en millis() para que la lectura del sensor y el control del buzzer no interfieran entre sí.

  Conexiones recomendadas:
    - Sensor IR KY‑033
        VCC  -> 5 V
        GND  -> GND
        OUT  -> D3 (entrada digital)
    - Buzzer activo de 5 V
        +    -> D2 (salida digital)
        –    -> GND

  Si tu buzzer es pasivo o consume más de 20 mA, usa un transistor NPN y un diodo en antiparalelo.
*/

#include <Arduino.h>

// Definición de pines
const uint8_t PIN_BUZZER = 2;  // Pin donde se conecta el buzzer
const uint8_t PIN_SENSOR = 3;  // Pin donde se conecta la salida digital del sensor IR

// Parámetros ajustables
const unsigned long OPEN_BLINK_MS  = 50;    // Duración del beep cuando la ventana está abierta
const unsigned long TAMPER_WINDOW_MS = 300;  // Ventana de tiempo para conteo de cambios (tamper)
const uint8_t TAMPER_MIN_EDGES = 5;         // Número mínimo de cambios para disparar la alarma tamper
const unsigned long TAMPER_HOLD_MS  = 15000; // Duración de la alarma tamper (15 s)
const unsigned long STABLE_MS       = 80;    // Tiempo mínimo de estabilidad para considerar el estado firme

// Variables de estado
bool lastRead       = HIGH;   // Última lectura instantánea del sensor (asumimos cerrado al iniciar)
bool stableState    = HIGH;   // Estado estabilizado (HIGH = negro, LOW = blanco)
unsigned long lastChange   = 0;  // Momento del último cambio de lectura instantánea
unsigned long stableSince  = 0;  // Momento desde que el estado está estable
uint8_t edgeCount    = 0;      // Contador de cambios en la ventana de tamper
unsigned long windowStart = 0;  // Inicio de la ventana de tamper
bool tamperActive    = false; // Indica si la alarma tamper está activada
unsigned long tamperSince  = 0;  // Momento en que se activó la alarma tamper
bool buzzerOn        = false; // Estado actual del buzzer (encendido/apagado) para parpadeo
unsigned long lastToggle  = 0;  // Momento de la última conmutación del buzzer para beep intermitente

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_SENSOR, INPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // Inicializa temporizadores
  unsigned long now = millis();
  lastChange  = now;
  stableSince = now;
  windowStart = now;
}

void loop() {
  unsigned long now = millis();

  // 1. Lectura del sensor y estabilización
  bool raw = digitalRead(PIN_SENSOR); // HIGH = negro (ventana cerrada), LOW = blanco (ventana abierta)

  // Detecta cambio instantáneo (edge)
  if (raw != lastRead) {
    lastRead = raw;
    lastChange = now;
    edgeCount++; // incrementa contador de cambios para tamper
  }

  // Actualiza estado estable si se mantiene el valor sin cambios durante STABLE_MS
  if (raw != stableState && (now - lastChange >= STABLE_MS)) {
    stableState = raw;
    stableSince = now;
  }

  // 2. Detección de tamper por oscilación rápida
  if (now - windowStart >= TAMPER_WINDOW_MS) {
    if (edgeCount >= TAMPER_MIN_EDGES) {
      tamperActive = true;
      tamperSince = now;
    }
    edgeCount = 0;
    windowStart = now;
  }
  // Desactiva la alarma tamper después del tiempo de latch
  if (tamperActive && (now - tamperSince >= TAMPER_HOLD_MS)) {
    tamperActive = false;
  }

  // 3. Selección del modo de alarma
  if (tamperActive) {
    // Alarma tamper: buzzer encendido continuo para máxima advertencia
    digitalWrite(PIN_BUZZER, HIGH);
    buzzerOn = true;
  } else if (stableState == LOW) {
    // Ventana abierta/movimiento: beep intermitente sin bloquear
    nonBlockingBlink(now, OPEN_BLINK_MS);
  } else {
    // Ventana cerrada: buzzer apagado
    buzzerOff();
  }
}

// Función auxiliar para parpadeo no bloqueante
void nonBlockingBlink(unsigned long now, unsigned long intervalMs) {
  if (now - lastToggle >= intervalMs) {
    lastToggle = now;
    buzzerOn = !buzzerOn;
    digitalWrite(PIN_BUZZER, buzzerOn ? HIGH : LOW);
  }
}

// Apaga el buzzer
void buzzerOff() {
  buzzerOn = false;
  digitalWrite(PIN_BUZZER, LOW);
}