// === main.cpp ===
#include "ethernet_manager.h"
#include "webserver.h"
#include "sensors.h"
#include "config.h"
#include "http-mqtt.h"

unsigned long lastSensorUpdate = 0;
const unsigned long sensorInterval = 1000;  // cada 1 segundo

void setup() {
  Serial.begin(115200);
  ethernetSetup();
  loadConfig();
  initSensors();
  initMQTT();
  startWebServer();
}

void loop() {
  ethernetLoop();
  handleClient();

  if (millis() - lastSensorUpdate >= sensorInterval) {
    lastSensorUpdate = millis();
    updateSensors();
  }
  actualizarComunicaciones();

  // Interfaz de consola serial
  static String inputString = "";
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      inputString.trim();
      if (inputString == "menu") {
        Serial.println("\n📋 MENÚ:");
        Serial.println("1) Imprimir datos de sensores");
        Serial.println("2) Imprimir dirección IP");
        Serial.println("3) Activar/desactivar HTTP");

        Serial.println("4) Activar/desactivar MQTT");
      } else if (inputString == "1") {
        Serial.printf("Sensor 1: %.2f %s\n", getSensor1Converted(), getSensor1Unit().c_str());
        Serial.printf("Sensor 2: %.2f %s\n", getSensor2Converted(), getSensor2Unit().c_str());
        Serial.printf("Sensor 3: %.2f %s\n", getSensor3Converted(), getSensor3Unit().c_str());
      } else if (inputString == "2") {
        Serial.print("IP actual: ");
        Serial.println(getLocalIP());
      } else if (inputString == "3") {
        enviarPorHTTP = !enviarPorHTTP;
        Serial.printf("HTTP %s\n", enviarPorHTTP ? "activado" : "desactivado");
      } else if (inputString == "4") {
        enviarPorMQTT = !enviarPorMQTT;
        Serial.printf("MQTT %s\n", enviarPorMQTT ? "activado" : "desactivado");
      } else {
        Serial.println("Comando no reconocido. Escribe 'menu'.");
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }
}
