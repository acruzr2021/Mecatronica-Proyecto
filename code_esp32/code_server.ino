#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

WebServer server(80); // Servidor en el puerto 80
Servo miServo;        // Objeto para controlar el servo
const int pinServo = 2; // Pin donde está conectado el servo

// Reemplaza con tus credenciales de Wi-Fi
const char* ssid = "MiFibra-E7CC";        // Nombre de tu red Wi-Fi
const char* password = "WZ9zFWxc";        // Contraseña de tu red Wi-Fi

void setup() {
  // Iniciar la comunicación serial
  Serial.begin(115200);

  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  // Una vez conectado
  Serial.println("¡Conexión exitosa!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP()); // Imprime la dirección IP asignada

  // Configurar rutas del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/encender", HTTP_GET, encenderMotor);
  server.on("/apagar", HTTP_GET, apagarMotor);
  server.on("/moverServo", HTTP_GET, moverServo);

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor web iniciado");

  // Inicializar el servo
  miServo.attach(pinServo);
}

void loop() {
  // Manejar solicitudes del cliente
  server.handleClient();
}

void handleRoot() {
  // Página HTML principal con slider y botones
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Control del Servo y Motor</title>
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
      .slider-container {
        display: flex;
        flex-direction: column;
        align-items: center;
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
      .button-container {
        display: flex;
        gap: 20px;
      }
      .button {
        padding: 15px 30px;
        font-size: 18px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 5px;
        text-decoration: none;
        text-align: center;
        cursor: pointer;
      }
      .button:hover {
        background-color: #0056b3;
      }
    </style>
    <script>
      function updateServo() {
        // Obtener el valor de la barra deslizante
        var angle = document.getElementById("slider").value;
        // Mostrar el valor en la página
        document.getElementById("output").innerText = "Angulo: " + angle;
        // Enviar el valor al servidor usando AJAX
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/moverServo?angle=" + angle, true);
        xhr.send();
      }

      function sendCommand(command) {
        // Enviar comando al servidor (encender o apagar motor)
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/" + command, true);
        xhr.send();
      }
    </script>
  </head>
  <body>
    <h1>Control del Servo y Motor</h1>
    <div class="slider-container">
      <!-- Barra deslizante -->
      <input type="range" id="slider" class="slider" min="0" max="180" value="90" oninput="updateServo()">
      <div id="output" class="output">Ángulo: 90°</div>
    </div>
    <div class="button-container">
      <!-- Botones para controlar el motor -->
      <button class="button" onclick="sendCommand('encender')">Encender Motor</button>
      <button class="button" onclick="sendCommand('apagar')">Apagar Motor</button>
    </div>
  </body>
  </html>
  )rawliteral";

  // Enviar la página HTML al cliente
  server.send(200, "text/html", html);
}

void encenderMotor() {
  // Código para encender el motor
  Serial.println("Motor encendido");
  server.send(204, "text/plain", ""); // Respuesta sin contenido
}

void apagarMotor() {
  // Código para apagar el motor
  Serial.println("Motor apagado");
  server.send(204, "text/plain", ""); // Respuesta sin contenido
}

void moverServo() {
  // Verificar si el parámetro "angle" está presente en la Asolicitud
  if (server.hasArg("angle")) {
    int angle = server.arg("angle").toInt(); // Convertir el valor a entero
    angle = constrain(angle, 0, 180);       // Limitar el ángulo entre 0 y 180 grados
    miServo.write(angle);                   // Mover el servo al ángulo especificado
    Serial.println("Ángulo recibido: " + String(angle)); // Imprimir en el monitor serie
    server.send(200, "text/plain", "Servo movido a " + String(angle) + "°");
  } else {
    // Si no se envió el parámetro, responder con un error
    server.send(400, "text/plain", "Parámetro 'angle' no encontrado");
  }
}
