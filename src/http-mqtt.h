// === http-mqtt.h ===
#ifndef HTTP_MQTT_H
#define HTTP_MQTT_H

#include <PubSubClient.h>
#include "config.h"
#include "sensors.h"
#include <ArduinoHttpClient.h>

// === Configuración de unidad y rango por sensor ===
float sensor1Min = 0.0, sensor1Max = 10.0;
float sensor2Min = 0.0, sensor2Max = 10.0;
float sensor3Min = 0.0, sensor3Max = 10.0;

String sensor1Unit = "bar";
String sensor2Unit = "grados_C";
String sensor3Unit = "metros";

// === Habilitar/deshabilitar comunicación ===
bool enviarPorHTTP = true;
bool enviarPorMQTT = true;

// === Configuración HTTP ===
String httpEndpoint = "/api/sensores";  // solo path
const char* httpServer = "192.168.1.18";   // dominio o IP
const int httpPort = 5002;

// === Configuración MQTT ===
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
const char* mqttServer = "test.mosquitto.org"; // dominio o IP
const int mqttPort = 1883;
String mqttTopic = "esp32/sensoresSmax";

// === Temporizador no bloqueante ===
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;  // 10 segundos

void initMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
}

float transformarVoltaje(float voltaje, float vMin, float vMax, float uMin, float uMax) {
  if (vMax == vMin) return uMin;
  return (voltaje - vMin) * (uMax - uMin) / (vMax - vMin) + uMin;
}

void enviarHTTP() {
  if (!enviarPorHTTP) return;

  HttpClient http(ethClient, httpServer, httpPort);
  String contentType = "application/json";

  float u1 = getSensor1Converted();
  float u2 = getSensor2Converted();
  float u3 = getSensor3Converted();

  String payload = "{";
  payload += "\"sensor1\":" + String(u1, 2) + ",\"unidad1\":\"" + sensor1Unit + "\",";
  payload += "\"sensor2\":" + String(u2, 2) + ",\"unidad2\":\"" + sensor2Unit + "\",";
  payload += "\"sensor3\":" + String(u3, 2) + ",\"unidad3\":\"" + sensor3Unit + "\"}";

  http.beginRequest();
  http.post(httpEndpoint);
  http.sendHeader("Content-Type", contentType);
  http.sendHeader("Content-Length", payload.length());
  http.beginBody();
  http.print(payload);
  http.endRequest();

  int statusCode = http.responseStatusCode();
  String response = http.responseBody();
  Serial.printf("[HTTP] Código: %d, Payload: %s\n", statusCode, payload.c_str());
  Serial.printf("[HTTP] Respuesta: %s\n", response.c_str());
}

void enviarMQTT() {
  if (!enviarPorMQTT) return;

  if (!mqttClient.connected()) {
    Serial.print("[MQTT] Conectando al broker...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("conectado.");
    } else {
      Serial.printf("falló, rc=%d\n", mqttClient.state());
      return;
    }
  }

  float u1 = getSensor1Converted();
  float u2 = getSensor2Converted();
  float u3 = getSensor3Converted();

  String payload = "{";
  payload += "\"sensor1\":" + String(u1, 2) + ",\"unidad1\":\"" + sensor1Unit + "\",";
  payload += "\"sensor2\":" + String(u2, 2) + ",\"unidad2\":\"" + sensor2Unit + "\",";
  payload += "\"sensor3\":" + String(u3, 2) + ",\"unidad3\":\"" + sensor3Unit + "\"}";

  mqttClient.publish(mqttTopic.c_str(), payload.c_str());
  Serial.printf("[MQTT] Publicado en %s: %s\n", mqttTopic.c_str(), payload.c_str());
}

void actualizarComunicaciones() {
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    if (enviarPorHTTP) enviarHTTP();
    if (enviarPorMQTT) enviarMQTT();
  }

  if (enviarPorMQTT) {
    mqttClient.loop();
  }
}

#endif
