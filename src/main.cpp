#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"
#include "UdpCalculator.h"
#include "TcpCalculator.h"
#include "TextCalculator.h"

UdpCalculator udpCalc(localPort);
TcpCalculator tcpCalc(localPort + 1);
TextCalculator textCalc(localPort + 2, localPort + 3);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi
  Serial.printf("\nConnecting to Wi-Fi: %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the services
  udpCalc.begin();
  tcpCalc.begin();
  textCalc.begin();
}

void loop() {
  udpCalc.handlePackets();
  tcpCalc.handleClients();
  textCalc.handleUdp();
  textCalc.handleTcp();
}