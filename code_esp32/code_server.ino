#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>
WebServer server(80);

Servo miServo;
const int pinServo = 2;

// Reemplaza con tus credenciales de Wi-Fi
const char* ssid = "MiFibra-E7CC";        // Nombre de tu red Wi-Fi
const char* password = "WZ9zFWxc";  // Contraseña de tu red Wi-Fi

void setup() {
  // Iniciar la comunicación serial
  Serial.begin(115200);
  
  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);

  // Esperar hasta que se conecte
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  // Una vez conectado
  Serial.println("¡Conexión exitosa!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());  // Imprime la dirección IP asignada

  // Rutas del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/encender", HTTP_GET, encenderMotor);
  server.on("/apagar", HTTP_GET, apagarMotor);

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor web iniciado");

  // Configuración inicial del servo
  // miServo.attach(pinServo);
}

void loop() {
  // Manejar solicitudes del cliente
  server.handleClient();
}

void handleRoot() {
  // Página HTML principal con AJAX
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Control de Motor</title>
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
      button {
        font-size: 20px;
        padding: 15px 30px;
        margin: 10px;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }
      button#encender {
        background-color: green;
        color: white;
      }
      button#apagar {
        background-color: red;
        color: white;
      }
    </style>
    <script>
      function sendCommand(command) {
        // Crear solicitud AJAX
        var xhr = new XMLHttpRequest();
        xhr.open("GET", command, true);
        xhr.send();
      }
    </script>
  </head>
  <body>
    <h1>Control de Motor</h1>
    <!-- Botones que llaman la función sendCommand con la ruta adecuada -->
    <button id="encender" onclick="sendCommand('/encender')">Encender Motor</button>
    <button id="apagar" onclick="sendCommand('/apagar')">Apagar Motor</button>
  </body>
  </html>
  )rawliteral";

  // Enviar la página HTML al cliente
  server.send(200, "text/html", html);
}

void encenderMotor() {
  // Encender motor (descomenta la línea adecuada para tu hardware)
  // digitalWrite(pinServo, HIGH);
  server.send(200, "text/plain", "Motor encendido");
  Serial.println("Motor encendido");
}

void apagarMotor() {
  // Apagar motor (descomenta la línea adecuada para tu hardware)
  // digitalWrite(pinServo, LOW);
  server.send(200, "text/plain", "Motor apagado");
  Serial.println("Motor apagado");
}
