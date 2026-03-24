#include "TcpCalculator.h"
#include "Config.h"

TcpCalculator::TcpCalculator(unsigned int port) : _server(port), _port(port) {}

void TcpCalculator::begin() {
  _server.begin();
  Serial.print("TCP Server listening on port: ");
  Serial.println(_port);
}

void TcpCalculator::handleClients() {
  WiFiClient client = _server.available();
  if (client) {
    processClient(client);
  }
}

void TcpCalculator::processClient(WiFiClient& client) {
  // Wait for data
  unsigned long timeout = millis();
  while (client.available() < (int)sizeof(CalcPacket) && (millis() - timeout < 100)) {
    delay(1);
  }

  if (client.available() >= (int)sizeof(CalcPacket)) {
    CalcPacket p;
    client.read((uint8_t*)&p, sizeof(CalcPacket));

    if (p.header == 0xAA && p.checksum == calculateChecksum(p)) {
      float result = 0;
      switch (p.op) {
        case 1: result = p.val1 + p.val2; break;
        case 2: result = p.val1 * p.val2; break;
        case 3: result = p.val1 - p.val2; break;
      }

      ResPacket res;
      res.header = 0xBB;
      res.result = result;
      res.checksum = calculateResChecksum(res);

      client.write((uint8_t*)&res, sizeof(ResPacket));
      
      // Blink LED
      digitalWrite(ledPin, HIGH);
      delay(1);
      digitalWrite(ledPin, LOW);
    }
  }
  client.stop();
}

uint8_t TcpCalculator::calculateChecksum(const CalcPacket& p) {
  uint8_t* ptr = (uint8_t*)&p;
  uint8_t sum = 0;
  for (size_t i = 0; i < sizeof(CalcPacket) - 1; i++) {
    sum += ptr[i];
  }
  return sum;
}

uint8_t TcpCalculator::calculateResChecksum(const ResPacket& p) {
  uint8_t* ptr = (uint8_t*)&p;
  uint8_t sum = 0;
  for (size_t i = 0; i < sizeof(ResPacket) - 1; i++) {
    sum += ptr[i];
  }
  return sum;
}
