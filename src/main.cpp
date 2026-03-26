#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"
#include "ActivationService.h"

ActivationService actService(p2pPort);

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

  // Start the service
  actService.begin();
}

void loop() {
  actService.handlePackets();
}