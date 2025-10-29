Sistema de alarma para ventanas con sensor infrarrojo (IR) y buzzer
¿Qué hace el proyecto?
Este proyecto implementa una alarma para ventanas que aprovecha un sensor de seguimiento de línea por infrarrojos (modelo KY‑033) y un buzzer para avisar cuando se detecta un movimiento o apertura de la ventana.
El sensor IR incorpora un diodo emisor y un receptor infrarrojo, junto con un comparador LM393 y un potenciómetro de ajuste[1]. El módulo tiene tres pines: VCC, GND y OUT, y su salida digital se pone en nivel alto (HIGH) cuando está sobre una superficie negra (detectando la marca de ventana cerrada) y en nivel bajo (LOW) cuando está sobre una superficie blanca o reflectante (ventana abierta)[2].
Al monitorear ese estado y generar distintas secuencias de activación del buzzer se construye un sistema antirrobo simple y de bajo coste (≈5–10 USD), pensado para que cualquier persona pueda replicarlo en 2–4 horas[3].
Problema que resuelve
Los sensores de vibración pueden fallar cuando la ventana se abre sin sacudidas o provocar falsas alarmas por ruidos ambientales. Usar un sensor IR que diferencie entre negro y blanco permite detectar la apertura real de la hoja sin verse afectado por vibraciones, con lo que se reduce el número de falsas alarmas y se obtiene una señal más fiable. El sistema también detecta manipulaciones rápidas (tamper) producidas por golpes o cambios bruscos en la señal y activa una alarma fuerte.
Propósito, contexto y alcance
El propósito principal es aportar un sistema de seguridad doméstica fácil de montar y de comprender. Está dirigido a estudiantes y aficionados que quieran aprender a usar sensores, microcontroladores y a documentar sus proyectos. Su alcance se centra en:
•	Detectar si una ventana pasa de cerrada a abierta (o viceversa) mediante un cambio en el color de referencia (negro/blanco).
•	Generar una alarma sonora diferente para los estados de apertura y tamper.
•	Ofrecer un código base flexible que pueda adaptarse a otros sensores (magnéticos, vibración, etc.).
¿Cómo funciona internamente?
Arquitectura general
El sistema está formado por tres bloques principales:
1.	Sensor IR (KY‑033) – emite y detecta luz infrarroja. El comparador integrado entrega una señal digital: HIGH cuando se detecta negro y LOW cuando se detecta blanco[2]. El potenciómetro del módulo permite ajustar la sensibilidad[1].
2.	Microcontrolador Arduino Nano – lee la señal del sensor en el pin digital D3, procesa la lógica (estado estable, conteo de cambios para tamper) y controla el buzzer en el pin digital D2. Se utiliza C++/Arduino a través del Arduino IDE para compilar y cargar el firmware.
3.	Buzzer – emite la alarma sonora. Un buzzer activo (suena al aplicar 5 V) conectado a D2 es suficiente. Si el buzzer es pasivo o consume >20 mA se aconseja añadir un transistor y un diodo de protección.
El diagrama siguiente resume el flujo de información entre estos módulos. Los parámetros (tiempos de parpadeo, número de transiciones para tamper y duración de la alarma) se definen en constantes para facilitar su ajuste.
 

Software y lógica de detección
El firmware se divide en tres tareas principales:
1.	Lectura y filtrado de la entrada – se lee el pin D3 a intervalos rápidos (sin bloquear) y se compara con la lectura anterior. Cuando la señal cambia de estado (borde) se incrementa un contador y se actualiza un temporizador. Se aplica un filtro de estabilidad (por ejemplo 80 ms) antes de considerar que el estado es firme, lo que evita saltos espurios.
2.	Detección de tamper – se utiliza una ventana deslizante de unos cientos de milisegundos (por ejemplo 300 ms) para contar cuántos cambios han ocurrido. Si el número de cambios supera un umbral (por ejemplo 5) se interpreta como un golpe o manipulación y se activa una alarma fuerte durante un período fijo (latched), independientemente de que la señal se estabilice.
3.	Gestión del buzzer – según el estado establecido o la activación de tamper se llaman funciones para parpadear el buzzer o mantenerlo encendido. La lógica se basa en variables temporales y no bloquea la lectura del sensor.
Estructura de carpetas recomendada
Aunque un proyecto sencillo de Arduino puede consistir en un único archivo .ino, para favorecer la extensibilidad y la colaboración se recomienda la siguiente organización:
alarma_ir/
├── README.md               # Documentación y guía de uso
├── src/
│   └── alarma_ir.ino       # Código principal (setup, loop, funciones)
├── lib/
│   └── ...                 # Librerías auxiliares (ej. manejo de buzzer, filtrado)
├── docs/
│   └── diagramas/          # Diagramas de arquitectura y secuencia
├── hardware/
│   ├── esquemático.pdf     # Esquemático eléctrico y pinout
│   └── lista_materiales.md # Lista de componentes y enlaces
├── tests/
│   └── sensor_test.ino     # Ejemplos de prueba para sensor y buzzer
└── LICENSE
Tecnologías usadas
•	Arduino IDE / PlatformIO – entorno de desarrollo que compila y carga código C++ al microcontrolador.
•	Librerías de Arduino – se utiliza pinMode, digitalRead, digitalWrite, millis y eventualmente tone() para control de buzzer.
•	CH340 Driver – muchos clones de Arduino usan el chip CH340 para la conexión USB; asegúrate de instalar el controlador para que el PC detecte el puerto serie.
•	Git – para controlar versiones y facilitar contribuciones.
Comunicación entre módulos y decisiones técnicas
•	El sensor IR se conecta a la placa mediante tres cables. De acuerdo con la especificación del KY‑033, se alimenta a 3,3–5 V, se conecta GND a la masa común y su salida digital OUT se conecta a un pin digital del Arduino[4].
•	Se decidió emplear un sensor de reflexión en lugar de uno de vibración porque su salida digital diferenciada (HIGH en negro y LOW en blanco) produce señales más limpias y menos susceptibles a falsos positivos.
•	Se implementó la detección de tamper mediante un contador de transiciones dentro de una ventana temporal, pues golpes o manipulaciones rápidas provocan múltiples cambios entre HIGH y LOW. Esta lógica se ajusta con constantes sin modificar la estructura general del código.
•	El uso de millis() en lugar de delay() permite ejecutar varias tareas en paralelo (lectura de sensor, detección de tamper y parpadeo de buzzer) sin bloquear el programa.
•	Para documentar y compartir el proyecto se recomienda un README detallado y diagramas de arquitectura y secuencia.
¿Cómo se usa o contribuye alguien más?
Paso 1 – Clonar el repositorio
git clone https://github.com/usuario/alarma_ir.git
cd alarma_ir
Paso 2 – Montar el hardware
1.	Componentes: Arduino Nano (ATmega328P con CH340), sensor KY‑033 (o equivalente), buzzer activo, resistencia de 220 Ω para LED de test, cables.
2.	Conexiones básicas:
3.	VCC del sensor → 5 V del Arduino.
4.	GND del sensor → GND común.
5.	OUT del sensor → pin digital D3.
6.	Buzzer (+) → pin digital D2; (–) → GND. Si el buzzer consume >20 mA, intercalar un transistor NPN y un diodo de rueda libre.
7.	Marca negra/blanca: coloca una cinta negra sobre el marco de la ventana para representar el estado cerrado; al abrir, el sensor verá una superficie clara y cambiará su salida.
8.	Ajusta el potenciómetro del sensor para que la salida cambie de estado cuando el sensor pase de negro a blanco[1].
Paso 3 – Instalar dependencias
1.	Instala Arduino IDE (versión ≥2.x) desde arduino.cc.
2.	Instala el driver CH340 si tu placa no aparece en la lista de puertos.
3.	(Opcional) Instala PlatformIO si prefieres un entorno de desarrollo integrado en VS Code.
4.	No se requieren librerías externas: el proyecto usa únicamente funciones estándar de Arduino.
Paso 4 – Compilar y cargar el firmware
1.	Abre src/alarma_ir.ino con Arduino IDE.
2.	Ve a Herramientas → Placa y selecciona Arduino Nano; en Procesador elige ATmega328P o ATmega328P (Old Bootloader) según tu placa.
3.	Selecciona el puerto serie correspondiente al CH340 (normalmente aparece como USB‑SERIAL CH340).
4.	Haz clic en Verificar para compilar y luego en Cargar. La subida tarda ~2–6 segundos.
5.	Abre el Monitor serie (9600 baudios) para ver las lecturas de stableState y tamper durante las pruebas.
Paso 5 – Probar el sistema
1.	Coloca el sensor sobre la marca negra (ventana cerrada). Deberías ver mensajes de “cerrado” y escuchar el buzzer apagado.
2.	Mueve la ventana lentamente hasta que el sensor vea la zona blanca: el buzzer debería parpadear con períodos de 50 ms.
3.	Golpea suavemente el sensor o alterna la posición negro/blanco varias veces en menos de 300 ms: el código detectará tamper y encenderá el buzzer de forma continua durante 15 s.
4.	Ajusta los tiempos y umbrales en el archivo .ino si necesitas mayor o menor sensibilidad.
Cómo correr pruebas y servidores
El proyecto no incluye un servidor, pero sí puede contener ejemplos de prueba en la carpeta tests/. Para verificar individualmente el sensor o el buzzer, abre el archivo tests/sensor_test.ino, selecciona la placa y carga el programa. Este ejemplo imprimirá “Line detected” cuando el sensor vea negro y “Line NOT detected” cuando vea blanco[5], lo que facilita su calibración.
Cómo contribuir
Si decides abrir el proyecto como software libre, agrega un archivo CONTRIBUTING.md basado en guías como la Contributor Covenant[6]. Incluye normas para reportar fallos, sugerir mejoras y enviar pull requests. Invita a los colaboradores a:
•	Mantener el estilo de código (nombres descriptivos, comentarios claros).
•	Añadir o actualizar pruebas cuando modifiquen la lógica.
•	Documentar cualquier cambio en el README y añadir diagramas si alteran la arquitectura.
•	Respetar la licencia elegida (por ejemplo GPL o MIT)[7].
¿Cómo empiezo a documentar?
Estructura mínima de un README
Un buen README responde a las preguntas qué, por qué y cómo. Según las buenas prácticas documentadas por freeCodeCamp, conviene estructurarlo así:
1.	Título del proyecto – una frase que describa el objetivo del proyecto[8].
2.	Descripción – explica qué hace la aplicación, por qué elegiste ciertas tecnologías y qué retos encontraste[9]. Responde preguntas como: ¿qué problema resuelve?, ¿qué aprendiste?[10].
3.	Tabla de contenidos (opcional) – si el documento es largo, ayuda a navegar[11].
4.	Instalación y ejecución – incluye los pasos y dependencias necesarias para preparar el entorno y compilar[12].
5.	Uso – proporciona instrucciones y ejemplos para que otros sepan utilizar el proyecto; se recomienda incluir capturas de pantalla o diagramas para ilustrar el funcionamiento[13].
6.	Créditos – menciona a los colaboradores y materiales de referencia[14].
7.	Licencia – indica bajo qué términos se comparte el código[15].
8.	Cómo contribuir y pruebas – describe las normas para contribuir y cómo ejecutar las pruebas automáticas[16].
Recursos visuales y técnicos
Además del README básico, la documentación debería incluir:
•	Diagramas de arquitectura – como el que se muestra en este informe, para dar una visión general del sistema.
•	Diagramas de secuencia – útiles cuando existen flujos complejos (por ejemplo, comunicación entre múltiples sensores).
•	Esquemáticos y listas de materiales – dibujados en KiCad o Fritzing para mostrar las conexiones eléctricas.
•	Resumen técnico – tabla con parámetros relevantes (tensión de funcionamiento, pines usados, tiempos de detección, etc.).
•	Guía de instalación paso a paso – los pasos anteriores en un apartado del README.
•	FAQ – sección con preguntas frecuentes, como: “¿Por qué mi buzzer no suena?” (responde: comprueba la polaridad y la corriente), “¿Cómo ajustar la sensibilidad del sensor?” (gira el potenciómetro hasta que la salida cambie entre HIGH y LOW), o “¿Qué hacer si la carga del programa falla?” (selecciona el procesador correcto o prueba el bootloader antiguo).
Resumen técnico del sistema
Elemento	Descripción breve
Microcontrolador	Arduino Nano (ATmega328P) con conversor CH340.
Sensor IR	KY‑033 line tracking; emite y recibe IR; salida digital HIGH en negro y LOW en blanco[2]; funciona entre 3,3–5,5 V[17].

Alarma	Buzzer activo de 5 V conectado al pin D2; se puede sustituir por sirena con transistor si se requiere mayor potencia.
Estrategia de detección	Lectura no bloqueante, filtro de estabilidad (80 ms), detección de tamper (≥5 cambios en 300 ms) y control de buzzer.
Coste estimado	5–10 USD, dependiendo del sensor y buzzer.
Nivel de dificultad	Principiantes/intermedios; la guía paso a paso facilita la construcción[3].

Con estos elementos y guía de documentación, cualquier persona debería poder clonar el repositorio, montar el hardware, compilar el código y poner en marcha la alarma sin necesidad de asistencia adicional. Además, la estructura del proyecto y los principios de documentación ayudan a que otras personas puedan contribuir, extender la funcionalidad o integrar nuevos sensores sin perderse.
FAQ – Preguntas frecuentes
Esta sección responde a las dudas más comunes que suelen surgir al montar o modificar el proyecto. Ajusta las respuestas según tu experiencia y añade nuevas preguntas a medida que surjan.
¿Por qué mi buzzer no suena?
Asegúrate de que usas un buzzer activo (que suene al aplicarle 5 V) y que está conectado correctamente (positivo a D2 y negativo a GND). Si tu buzzer es pasivo o un altavoz piezoeléctrico, necesitarás usar la función tone() o un circuito de excitación. Revisa también que el buzzer no consuma más de 20 mA; si lo hace, utiliza un transistor NPN y un diodo de protección para alimentarlo externamente.
¿Cómo ajusto la sensibilidad del sensor IR?
El sensor KY‑033 tiene un potenciómetro que permite calibrar el umbral del comparador LM393. Gira el potenciómetro con un destornillador mientras observas el LED indicador del módulo: debes conseguir que el LED se apague al apuntar a la marca negra (HIGH en la salida) y se encienda al apuntar a la superficie blanca (LOW en la salida)[2]. Coloca el sensor a la misma distancia que tendrá en la ventana real durante la calibración.
¿Por qué la carga del programa falla o tarda demasiado?
Asegúrate de seleccionar la placa correcta en Herramientas → Placa y el procesador adecuado (ATmega328P para la mayoría de los Nanos). Si recibes mensajes de programmer is not responding, prueba la opción ATmega328P (Old Bootloader). Comprueba también que el cable USB sea de datos y que el puerto COM detectado corresponda al CH340. La subida del firmware suele tardar menos de 6 segundos.
¿Cómo puedo alimentar un buzzer o sirena más potente?
No conectes directamente dispositivos que consuman más de 20 mA al pin digital. Para sirenas de 9–12 V o buzzers de alta potencia utiliza un transistor NPN o un MOSFET como interruptor, conectando el colector/drain al buzzer y colocando un diodo en antiparalelo si se trata de un elemento electromagnético. Controla la base/gate con el pin D2 a través de una resistencia de ~1 kΩ.
¿Cómo ver los estados en el monitor serie?
Incluye instrucciones en tu sketch para imprimir el valor de la variable stableState o los contadores de tamper en el puerto serie con Serial.begin(9600); en setup() y Serial.println() dentro de loop(). Luego abre el Monitor serie de Arduino IDE y ajusta la velocidad a 9600 baudios.
¿Puedo usar otros sensores en lugar del KY‑033?
Sí. La lógica del código se basa en leer un nivel digital (HIGH o LOW). Puedes reemplazar el KY‑033 por un reed switch magnético, un sensor de vibración o un sensor PIR, siempre que adaptes la condición de activación y ajustes el cableado. Si el nuevo sensor entrega una señal analógica, tendrás que usar analogRead() y definir un umbral de disparo.
________________________________________
[1] [2] [4] [5] [17] KY-033 Line Tracking Sensor Module - ArduinoModulesInfo
https://arduinomodules.info/ky-033-line-tracking-sensor-module/
[3] [6] [7] [8] [9] [10] [11] [12] [13] [14] [15] [16] How to Write a Good README File for Your GitHub Project
https://www.freecodecamp.org/news/how-to-write-a-good-readme-file/
