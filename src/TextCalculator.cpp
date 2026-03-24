#include "TextCalculator.h"
#include "Config.h"

TextCalculator::TextCalculator(unsigned int udpPort, unsigned int tcpPort) 
  : _udpPort(udpPort), _tcpPort(tcpPort), _server(tcpPort) {}

void TextCalculator::begin() {
  _udp.begin(_udpPort);
  _server.begin();
  Serial.printf("Text services: UDP:%d, TCP:%d\n", _udpPort, _tcpPort);
}

void TextCalculator::handleUdp() {
  int packetSize = _udp.parsePacket();
  if (packetSize > 0) {
    char buffer[128];
    int len = _udp.read(buffer, 127);
    if (len > 0) {
      buffer[len] = '\0';
      String request = String(buffer);
      String result = processRequest(request);
      
      _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
      _udp.print(result);
      _udp.endPacket();
    }
  }
}

void TextCalculator::handleTcp() {
  WiFiClient client = _server.available();
  if (client) {
    unsigned long timeout = millis();
    while (!client.available() && millis() - timeout < 100) delay(1);
    
    if (client.available()) {
      String request = client.readStringUntil('\n');
      String result = processRequest(request);
      client.println(result);
    }
    client.stop();
  }
}

String TextCalculator::processRequest(String request) {
  // Format: "OP,VAL1,VAL2" (e.g., "1,10.0,20.0")
  int firstComma = request.indexOf(',');
  int secondComma = request.indexOf(',', firstComma + 1);
  
  if (firstComma == -1 || secondComma == -1) return "ERROR";
  
  int op = request.substring(0, firstComma).toInt();
  float v1 = request.substring(firstComma + 1, secondComma).toFloat();
  float v2 = request.substring(secondComma + 1).toFloat();
  
  float res = 0;
  switch (op) {
    case 1: res = v1 + v2; break;
    case 2: res = v1 * v2; break;
    case 3: res = v1 - v2; break;
    default: return "ERROR";
  }

  // Blink LED
  digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(ledPin, LOW);
  
  return String(res, 2);
}
