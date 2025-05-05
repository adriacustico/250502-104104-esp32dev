// === main.cpp ===
#include "ethernet_manager.h"
#include "webserver.h"
#include "sensors.h"
#include "config.h"

unsigned long lastSensorUpdate = 0;
const unsigned long interval = 1000;

void setup() {
  Serial.begin(115200);
  ethernetSetup();
  loadConfig();
  initSensors();
}

void loop() {
  ethernetLoop();
  handleClient();

  if (millis() - lastSensorUpdate > interval) {
    lastSensorUpdate = millis();
    updateSensors();
  }

static String inputString = "";
if (Serial.available()) {
  char c = Serial.read();
  if (c == '\n' || c == '\r') {
    inputString.trim();
    if (inputString == "menu") {
      Serial.println("\n📋 MENÚ:");
      Serial.println("1) Imprimir datos de sensores");
      Serial.println("2) Imprimir dirección IP");
    } else if (inputString == "1") {
      Serial.println("🔍 Sensores:");
      Serial.printf("Sensor 1: %.2f %s\n", getSensor1Converted(), getSensor1Unit().c_str());
      Serial.printf("Sensor 2: %.2f %s\n", getSensor2Converted(), getSensor2Unit().c_str());
      Serial.printf("Sensor 3: %.2f %s\n", getSensor3Converted(), getSensor3Unit().c_str());
    } else if (inputString == "2") {
      Serial.print("🌐 IP actual: ");
      Serial.println(getLocalIP());
    } else {
      Serial.println("❌ Comando no reconocido. Escribe 'menu'.");
    }
    inputString = "";
  } else {
    inputString += c;
  }
}

}
