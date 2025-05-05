// === config.h ===
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

struct SensorConfig {
  float vMin;
  float vMax;
  float uMin;
  float uMax;
  String unidad;
};

SensorConfig sensor1Cfg = {0.0, 3.3, 0.0, 10.0, "bar"};
SensorConfig sensor2Cfg = {0.0, 3.3, 0.0, 100.0, "%"};
SensorConfig sensor3Cfg = {0.0, 3.3, 0.0, 5.0, "m"};

void loadConfig() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Fallo SPIFFS");
    return;
  }
  File file = SPIFFS.open("/config.txt", "r");
  if (!file) {
    Serial.println("No existe config, usando valores por defecto");
    return;
  }

  sensor1Cfg.vMin = file.parseFloat();
  sensor1Cfg.vMax = file.parseFloat();
  sensor1Cfg.uMin = file.parseFloat();
  sensor1Cfg.uMax = file.parseFloat();
  sensor1Cfg.unidad = file.readStringUntil('\n');

  sensor2Cfg.vMin = file.parseFloat();
  sensor2Cfg.vMax = file.parseFloat();
  sensor2Cfg.uMin = file.parseFloat();
  sensor2Cfg.uMax = file.parseFloat();
  sensor2Cfg.unidad = file.readStringUntil('\n');

  sensor3Cfg.vMin = file.parseFloat();
  sensor3Cfg.vMax = file.parseFloat();
  sensor3Cfg.uMin = file.parseFloat();
  sensor3Cfg.uMax = file.parseFloat();
  sensor3Cfg.unidad = file.readStringUntil('\n');

  file.close();
  Serial.println("Configuración cargada de SPIFFS");
}

void saveConfig() {
  File file = SPIFFS.open("/config.txt", "w");
  if (!file) {
    Serial.println("No se pudo guardar configuración");
    return;
  }

  file.printf("%f\n%f\n%f\n%f\n%s\n", sensor1Cfg.vMin, sensor1Cfg.vMax, sensor1Cfg.uMin, sensor1Cfg.uMax, sensor1Cfg.unidad.c_str());
  file.printf("%f\n%f\n%f\n%f\n%s\n", sensor2Cfg.vMin, sensor2Cfg.vMax, sensor2Cfg.uMin, sensor2Cfg.uMax, sensor2Cfg.unidad.c_str());
  file.printf("%f\n%f\n%f\n%f\n%s\n", sensor3Cfg.vMin, sensor3Cfg.vMax, sensor3Cfg.uMin, sensor3Cfg.uMax, sensor3Cfg.unidad.c_str());

  file.close();
  Serial.println("Configuración guardada en SPIFFS");
}

#endif
