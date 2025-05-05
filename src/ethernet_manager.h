// === ethernet_manager.h ===
#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <SPI.h>
#include <EthernetENC.h>
#include "webserver.h"

#define ENC_CS 5
#define ENC_RST 16

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Config default (puede sobreescribirse desde config.h)
IPAddress staticIP(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dnsAddr(8, 8, 8, 8);
bool useDHCP = true;

void ethernetSetup() {
  pinMode(ENC_RST, OUTPUT);
  digitalWrite(ENC_RST, LOW); delay(100);
  digitalWrite(ENC_RST, HIGH); delay(100);

  SPI.begin(18, 19, 23, ENC_CS);
  SPI.setFrequency(20000000);

  Ethernet.init(ENC_CS);

  int attempts = 0;
  bool dhcpSuccess = false;

  while (attempts < 5 && !dhcpSuccess) {
    Serial.print("Intentando DHCP (intento "); Serial.print(attempts + 1); Serial.println(")...");
    if (Ethernet.begin(mac)) {
      dhcpSuccess = true;
      break;
    }
    delay(2000);  // espera entre intentos
    attempts++;
  }

  if (!dhcpSuccess) {
    Serial.println("❌ DHCP falló, usando IP estática...");
    Ethernet.begin(mac, staticIP, dnsAddr, gateway, subnet);
  }

  delay(100);
  Serial.print("✅ IP asignada: ");
  Serial.println(Ethernet.localIP());
  startWebServer(); // Reinicia el servidor web en cada reconexión

}


void ethernetLoop() {
  Ethernet.maintain();
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Red caída. Reintentando...");
    ethernetSetup();
  }
}

bool isNetworkConnected() {
  return Ethernet.linkStatus() == LinkON;
}

IPAddress getLocalIP() {
  return Ethernet.localIP();
}

#endif
