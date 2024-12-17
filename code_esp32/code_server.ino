#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// Configuración de red
const char* ssid = "Robotech";
const char* password = "clv.robotech";

// Configuración de pines de los motores
const int pinMotor1 = 2;
const int pinMotor2 = 4;
const int pinMotor3 = 5;
const int pinMotor4 = 15; // Motor de 360°

WebServer server(80); // Servidor en el puerto 80

// Configuración de pines de los LEDs
const int ledNoConexion = 13;
const int ledConectado = 12;  
const int ledServidorIniciado = 14; 

// Objetos Servo
Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

// Variables de estado
int currentPosition1 = 0;
int currentPosition3 = 0; // Posición actual del motor 3
int current360Position = 0; // Posición simulada del motor 360°
int targetPosition1 = 0;
int targetPosition3 = 0; // Objetivo para el motor 3
boolean encendido = false; // Estado del motor
const int timePerDegree = 10; // Tiempo aproximado por grado (ajustar según pruebas)

// Configuración inicial
void setup() {
  Serial.begin(115200);

  // Configurar LEDs como salida
  pinMode(ledNoConexion, OUTPUT);
  pinMode(ledConectado, OUTPUT);
  pinMode(ledServidorIniciado, OUTPUT);

  // Encender el LED de "sin conexión"
  digitalWrite(ledNoConexion, HIGH);
  digitalWrite(ledConectado, LOW);
  digitalWrite(ledServidorIniciado, LOW);

  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  // Cambiar al LED de "conectado"
  digitalWrite(ledNoConexion, LOW);
  digitalWrite(ledConectado, HIGH);

  if (!MDNS.begin("esp32")) {
    Serial.println("Error al configurar mDNS");
    while (1) delay(1000);
  }
  Serial.println("mDNS configurado. Puedes acceder con: http://esp32.local");

  // Mostrar información de conexión
  Serial.println("¡Conexión exitosa!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/controlMotor2", HTTP_GET, controlMotor2);
  server.on("/encender", HTTP_GET, encenderMotor);
  server.on("/apagar", HTTP_GET, apagarMotor);
  server.on("/moverServo", HTTP_GET, moverServo);

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor web iniciado");

  // Cambiar al LED de "servidor web iniciado"
  digitalWrite(ledServidorIniciado, HIGH);

  // Inicializar los servos
  motor1.attach(pinMotor1);
  motor2.attach(pinMotor2);
  motor3.attach(pinMotor3);
  motor4.attach(pinMotor4);
}

// Manejo de solicitudes del servidor
void loop() {
  server.handleClient();
}

// Página principal del servidor
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Control de Servos y Motor</title>
    <style>
      body {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        font-family: Arial, sans-serif;
        height: 100vh;
        margin: 0;
      }
      h1 {
        margin-bottom: 20px;
      }
      .slider-container, .button-container {
        margin-bottom: 30px;
      }
      .slider {
        width: 500px;
        height: 20px;
        cursor: pointer;
      }
      .output {
        margin-top: 10px;
        font-size: 20px;
      }
      .button {
        padding: 15px 30px;
        font-size: 18px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        margin: 5px;
      }
      .button:hover {
        background-color: #0056b3;
      }
    </style>
  </head>
  <body>
    <h1>Control de Servos y Motor</h1>
    <div class="slider-container">
      <h3>Motor brazo 1</h3>
      <input type="range" id="slider1" class="slider" min="0" max="180" value="90" oninput="updateServo(1)">
      <div id="output1" class="output">Angulo: 90</div>
    </div>
    <div class="slider-container">
      <h3>Motor brazo 2</h3>
      <input type="range" id="slider3" class="slider" min="0" max="180" value="0" oninput="updateServo(3)">
      <div id="output3" class="output">Angulo: 0</div>
    </div>
    <div class="slider-container">
      <h3>Motor brazo 3</h3>
      <input type="range" id="slider4" class="slider" min="0" max="180" value="0" oninput="updateServo(4)">
      <div id="output4" class="output">Angulo: 0</div>
    </div>
    <div class="button-container">
      <h3>Control del End Effector</h3>
      <button class="button" onclick="controlMotor('moderadoIzq')">Abrir rapido</button>
      <button class="button" onclick="controlMotor('lentoIzq')">Abrir lento</button>
      <button class="button" onclick="controlMotor('parar')">Parar</button>
      <button class="button" onclick="controlMotor('lentoDer')">Cerrar Lento</button>
      <button class="button" onclick="controlMotor('moderadoDer')">Cerrar rapido</button>
    </div>
    <script>
      function updateServo(motorId) {
        const slider = document.getElementById("slider" + motorId);
        const angle = slider.value;
        document.getElementById("output" + motorId).innerText = "Angulo: " + angle;
        const xhr = new XMLHttpRequest();
        xhr.open("GET", "/moverServo?motor=" + motorId + "&angle=" + angle, true);
        xhr.send();
      }

      function controlMotor(command) {
        const xhr = new XMLHttpRequest();
        xhr.open("GET", "/controlMotor2?cmd=" + command, true);
        xhr.send();
      }
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Control del motor 2
void controlMotor2() {
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Parámetro 'cmd' requerido");
    return;
  }

  String cmd = server.arg("cmd");

  if (cmd == "parar") {
    motor2.write(90);
  } else if (cmd == "lentoIzq") {
    motor2.write(70);
  } else if (cmd == "moderadoIzq") {
    motor2.write(50);
  } else if (cmd == "lentoDer") {
    motor2.write(110);
  } else if (cmd == "moderadoDer") {
    motor2.write(130);
  } 

  server.send(200, "text/plain", "Comando ejecutado: " + cmd);
}

// Encender el motor
void encenderMotor() {
  Serial.println("Motor encendido");
  server.send(204, "text/plain", "");
  encendido = true;
}

// Apagar el motor
void apagarMotor() {
  Serial.println("Motor apagado");
  server.send(204, "text/plain", "");
  encendido = false;
}

// Mover los servos
void moverServo() {
  if (!server.hasArg("motor") || !server.hasArg("angle")) {
    server.send(400, "text/plain", "Parámetros 'motor' y 'angle' requeridos");
    return;
  }

  int motorId = server.arg("motor").toInt();
  int angle = server.arg("angle").toInt();
  angle = constrain(angle, 0, 360);

  if (motorId == 1) { 
    motor1.write(angle);
  
  } else if (motorId == 4) {
    targetPosition3 = angle;

    int delta = targetPosition3 - currentPosition3;
    int timeToMove = abs(delta) * 10;

    if (delta > 0) {
      motor4.write(180); // Gira en sentido horario
    } else if (delta < 0) {
      motor4.write(0);   // Gira en sentido antihorario
    }

    delay(timeToMove);   // Tiempo para completar el movimiento
    motor4.write(90);    // Detiene el movimiento
    currentPosition3 = targetPosition3;

  } else if (motorId == 3) {
    

    targetPosition1 = angle;

    int delta = targetPosition1 - currentPosition1;
    int timeToMove = abs(delta) * 4;

    if (delta > 0) {
      motor3.write(180); // Gira en sentido horario
    } else if (delta < 0) {
      motor3.write(0);   // Gira en sentido antihorario
    }

    delay(timeToMove);   // Tiempo para completar el movimiento
    motor3.write(90);    // Detiene el movimiento
    currentPosition1 = targetPosition1;

  }

  server.send(200, "text/plain", "Motor " + String(motorId) + " movido a " + String(angle) + "°");
}
