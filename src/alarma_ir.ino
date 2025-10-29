/*
  alarma_ir.ino – Arduino Nano (ATmega328P + CH340)
  Sensor IR (KY-033) en D3 | Buzzer en D2
  Estados:
   - Cerrado (NEGRO/HIGH): buzzer OFF
   - Abierto (BLANCO/LOW): parpadeo 50/50 ms
   - Tamper (≥5 cambios en ≤300 ms): buzzer ON continuo 15 s
*/

const uint8_t PIN_BUZZER = 2;   // D2
const uint8_t PIN_SENSOR = 3;   // D3

// Parámetros ajustables
const unsigned long OPEN_BLINK_MS    = 50;    // 50/50 ms
const unsigned long TAMPER_WINDOW_MS = 300;   // ventana de conteo
const uint8_t       TAMPER_MIN_EDGES = 5;     // edges para tamper
const unsigned long TAMPER_HOLD_MS   = 15000; // 15 s latch
const unsigned long STABLE_MS        = 80;    // estabilidad

// Estado interno
bool lastRead             = HIGH;  // asumimos cerrada al arrancar
bool stableState          = HIGH;
unsigned long lastChange  = 0;
unsigned long stableSince = 0;

uint8_t       edgeCount   = 0;
unsigned long windowStart = 0;

bool          tamperActive  = false;
unsigned long tamperSince   = 0;

bool          buzzerOn     = false;
unsigned long lastToggle   = 0;

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_SENSOR, INPUT);   // KY-033 entrega salida digital firme
  digitalWrite(PIN_BUZZER, LOW);

  unsigned long now = millis();
  lastChange   = now;
  stableSince  = now;
  windowStart  = now;

  // (Opcional) Serial para depurar:
  // Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // 1) Lectura y edges
  bool raw = digitalRead(PIN_SENSOR); // HIGH=NEGRO(cerrado), LOW=BLANCO(abierto)
  if (raw != lastRead) {
    lastRead = raw;
    lastChange = now;
    edgeCount++;
  }

  // 2) Estabilidad
  if (raw != stableState && (now - lastChange >= STABLE_MS)) {
    stableState = raw;
    stableSince = now;
  }

  // 3) Ventana de tamper
  if (now - windowStart >= TAMPER_WINDOW_MS) {
    if (edgeCount >= TAMPER_MIN_EDGES) {
      tamperActive = true;
      tamperSince = now;
    }
    edgeCount = 0;
    windowStart = now;
  }
  if (tamperActive && (now - tamperSince >= TAMPER_HOLD_MS)) {
    tamperActive = false;
  }

  // 4) Salida buzzer
  if (tamperActive) {
    // Alarma fuerte: ON continuo (máximo volumen en buzzer activo)
    digitalWrite(PIN_BUZZER, HIGH);
    buzzerOn = true;
  } else if (stableState == LOW) {
    // Abierto (BLANCO): parpadeo 50/50 ms
    nonBlockingBlink(now, OPEN_BLINK_MS);
  } else {
    buzzerOff();
  }
}

void nonBlockingBlink(unsigned long now, unsigned long intervalMs) {
  if (now - lastToggle >= intervalMs) {
    lastToggle = now;
    buzzerOn = !buzzerOn;
    digitalWrite(PIN_BUZZER, buzzerOn ? HIGH : LOW);
  }
}

void buzzerOff() {
  buzzerOn = false;
  digitalWrite(PIN_BUZZER, LOW);
}
