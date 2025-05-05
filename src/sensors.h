// === sensors.h ===
#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <esp_adc_cal.h>

const int SENSOR_1_PIN = 34;
const int SENSOR_2_PIN = 33;
const int SENSOR_3_PIN = 32;

esp_adc_cal_characteristics_t adc_chars;
const uint32_t DEFAULT_VREF = 3300;  // mV
const float alpha = 0.1;              // media móvil exponencial
const float detectionThreshold = 0.05; // voltaje mínimo para considerar sensor activo (50 mV)

float voltage1 = 0.0;
float voltage2 = 0.0;
float voltage3 = 0.0;

float filtro1 = 0.0;
float filtro2 = 0.0;
float filtro3 = 0.0;

bool sensor1Activo = false;
bool sensor2Activo = false;
bool sensor3Activo = false;

void initSensors() {
  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);

  analogSetAttenuation(ADC_11db); // 0–3.3V rango para ADC1
  analogReadResolution(12);       // resolución de 12 bits
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
}

float readVoltage(int pin) {
  const int N = 100;
  uint32_t sum = 0;
  for (int i = 0; i < N; i++) {
    sum += analogRead(pin);
  }
  uint32_t rawAvg = sum / N;
  uint32_t mv = esp_adc_cal_raw_to_voltage(rawAvg, &adc_chars);
  return mv / 1000.0; // convertir a voltios
}

void updateSensors() {
  float lectura1 = readVoltage(SENSOR_1_PIN);
  float lectura2 = readVoltage(SENSOR_2_PIN);
  float lectura3 = readVoltage(SENSOR_3_PIN);

  filtro1 = alpha * lectura1 + (1 - alpha) * filtro1;
  filtro2 = alpha * lectura2 + (1 - alpha) * filtro2;
  filtro3 = alpha * lectura3 + (1 - alpha) * filtro3;

  sensor1Activo = filtro1 > detectionThreshold;
  sensor2Activo = filtro2 > detectionThreshold;
  sensor3Activo = filtro3 > detectionThreshold;

  voltage1 = filtro1;
  voltage2 = filtro2;
  voltage3 = filtro3;

//  Serial.printf("[Filtrado] V1: %.2f V (%s), V2: %.2f V (%s), V3: %.2f V (%s)\n",
//                voltage1, sensor1Activo ? "activo" : "inactivo",
//                voltage2, sensor2Activo ? "activo" : "inactivo",
//                voltage3, sensor3Activo ? "activo" : "inactivo");
}

float convertToUnit(float voltage, SensorConfig cfg) {
  float ratio = (voltage - cfg.vMin) / (cfg.vMax - cfg.vMin);
  return cfg.uMin + ratio * (cfg.uMax - cfg.uMin);
}

float getSensor1Converted() { return convertToUnit(voltage1, sensor1Cfg); }
float getSensor2Converted() { return convertToUnit(voltage2, sensor2Cfg); }
float getSensor3Converted() { return convertToUnit(voltage3, sensor3Cfg); }

String getSensor1Unit() { return sensor1Cfg.unidad; }
String getSensor2Unit() { return sensor2Cfg.unidad; }
String getSensor3Unit() { return sensor3Cfg.unidad; }

#endif
