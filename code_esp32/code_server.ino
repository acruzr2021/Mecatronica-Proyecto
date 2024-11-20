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

  server.on("/", HTTP_GET, handleRoot);
  server.on("/encender", HTTP_GET, encenderMotor);
  server.on("/apagar", HTTP_GET, apagarMotor);

  server.begin();
  Serial.println("Servidor web iniciado");

  //miServo.attach(pinServo);
}

void loop() {
  
  
  server.handleClient();
  //for (int pos = 0; pos <= 360; pos += 1) {
  //  miServo.write(pos);
  //  delay(15);
  //}

  //miServo.write(0);    // Mover el servo a 0 grados
  //delay(1000);         // Esperar 1 segundo
  //miServo.write(90);   // Mover el servo a 90 grados
  //delay(1000);         // Esperar 1 segundo
  //miServo.write(200);  // Mover el servo a 180 grados
  //delay(1000);    
}

void handleRoot() {
  String html = "<html><body><h1>Control de Motor</h1>";
  html += "<button><a href='/encender'>Encender Motor</a></button><br>";
  html += "<button><a href='/apagar'>Apagar Motor</a></button><br>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void encenderMotor() {
  //digitalWrite(pinServo, HIGH);  // Encender motor
  server.send(200, "text/html", "<html><body><h1>Motor Encendido</h1><a href='/'>Volver</a></body></html>");
  Serial.println("Motor encendido");

}

void apagarMotor() {
  //digitalWrite(pinServo, LOW);  // Apagar motor
  server.send(200, "text/html", "<html><body><h1>Motor Apagado</h1><a href='/'>Volver</a></body></html>");
  Serial.println("Motor apagado");
}
