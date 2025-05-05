// === webserver.h ===
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <EthernetENC.h>
#include "sensors.h"
#include "config.h"

EthernetServer server(80);

void startWebServer() {
  server.begin();
  Serial.println("Servidor web iniciado en puerto 80");
}

void handleClient() {
  EthernetClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\n');
  String formData = "";
  while (client.available()) {
    formData += (char)client.read();
  }

  if (request.indexOf("GET /api/sensores") >= 0) {
    float s1 = getSensor1Converted();
    float s2 = getSensor2Converted();
    float s3 = getSensor3Converted();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("Connection: close");
    client.println();
    client.print("{\"s1\":"); client.print(isnan(s1) ? 0.0 : s1);
    client.print(",\"u1\":\""); client.print(getSensor1Unit());
    client.print("\",\"s2\":"); client.print(isnan(s2) ? 0.0 : s2);
    client.print(",\"u2\":\""); client.print(getSensor2Unit());
    client.print("\",\"s3\":"); client.print(isnan(s3) ? 0.0 : s3);
    client.print(",\"u3\":\""); client.print(getSensor3Unit());
    client.println("\"}");
    delay(1); client.stop(); return;
  }

  if (request.indexOf("POST /config") >= 0) {
    sensor1Cfg.vMin = formData.substring(formData.indexOf("vMin1=")+6).toFloat();
    sensor1Cfg.vMax = formData.substring(formData.indexOf("vMax1=")+6).toFloat();
    sensor1Cfg.uMin = formData.substring(formData.indexOf("uMin1=")+6).toFloat();
    sensor1Cfg.uMax = formData.substring(formData.indexOf("uMax1=")+6).toFloat();
    sensor1Cfg.unidad = formData.substring(formData.indexOf("unidad1=")+8, formData.indexOf("&vMin2=")).c_str();

    sensor2Cfg.vMin = formData.substring(formData.indexOf("vMin2=")+6).toFloat();
    sensor2Cfg.vMax = formData.substring(formData.indexOf("vMax2=")+6).toFloat();
    sensor2Cfg.uMin = formData.substring(formData.indexOf("uMin2=")+6).toFloat();
    sensor2Cfg.uMax = formData.substring(formData.indexOf("uMax2=")+6).toFloat();
    sensor2Cfg.unidad = formData.substring(formData.indexOf("unidad2=")+8, formData.indexOf("&vMin3=")).c_str();

    sensor3Cfg.vMin = formData.substring(formData.indexOf("vMin3=")+6).toFloat();
    sensor3Cfg.vMax = formData.substring(formData.indexOf("vMax3=")+6).toFloat();
    sensor3Cfg.uMin = formData.substring(formData.indexOf("uMin3=")+6).toFloat();
    sensor3Cfg.uMax = formData.substring(formData.indexOf("uMax3=")+6).toFloat();
    sensor3Cfg.unidad = formData.substring(formData.indexOf("unidad3=")+8).c_str();

    saveConfig();
    client.println("HTTP/1.1 303 See Other");
    client.println("Location: /");
    client.println();
    delay(1); client.stop(); return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<title>ESP32 Web</title>");
  client.println("<script>");
  client.println("function actualizar() {");
  client.println("  fetch('/api/sensores')");
  client.println("    .then(res => res.json())");
  client.println("    .then(data => {");
  client.println("      if (!isNaN(data.s1)) document.getElementById('s1').innerText = data.s1 + ' ' + data.u1;");
  client.println("      if (!isNaN(data.s2)) document.getElementById('s2').innerText = data.s2 + ' ' + data.u2;");
  client.println("      if (!isNaN(data.s3)) document.getElementById('s3').innerText = data.s3 + ' ' + data.u3;");
  client.println("    });");
  client.println("}");
  client.println("setInterval(actualizar, 2000);");
  client.println("window.onload = actualizar;");
  client.println("</script></head><body><h2>Sensores</h2>");
  client.println("<ul>");
  client.println("<li>Sensor 1: <span id='s1'></span></li>");
  client.println("<li>Sensor 2: <span id='s2'></span></li>");
  client.println("<li>Sensor 3: <span id='s3'></span></li>");
  client.println("</ul>");

  client.println("<h2>Configuración</h2>");
  client.println("<form action='/config' method='POST'>");
  client.println("<h3>Sensor 1</h3>");
  client.println("VMin: <input name='vMin1'> VMax: <input name='vMax1'><br>");
  client.println("UMin: <input name='uMin1'> UMax: <input name='uMax1'><br>");
  client.println("Unidad: <input name='unidad1'><br>");

  client.println("<h3>Sensor 2</h3>");
  client.println("VMin: <input name='vMin2'> VMax: <input name='vMax2'><br>");
  client.println("UMin: <input name='uMin2'> UMax: <input name='uMax2'><br>");
  client.println("Unidad: <input name='unidad2'><br>");

  client.println("<h3>Sensor 3</h3>");
  client.println("VMin: <input name='vMin3'> VMax: <input name='vMax3'><br>");
  client.println("UMin: <input name='uMin3'> UMax: <input name='uMax3'><br>");
  client.println("Unidad: <input name='unidad3'><br>");

  client.println("<input type='submit' value='Guardar Configuración'>");
  client.println("</form>");
  client.println("</body></html>");

  delay(1);
  client.stop();
}

#endif