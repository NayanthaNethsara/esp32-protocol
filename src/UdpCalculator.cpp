#include "UdpCalculator.h"
#include "Config.h"

UdpCalculator::UdpCalculator(unsigned int port) : _port(port) {}

void UdpCalculator::begin() {
  _udp.begin(_port);
  Serial.print("UDP Listening on port: ");
  Serial.println(_port);
}

void UdpCalculator::handlePackets() {
  int packetSize = _udp.parsePacket();
  if (packetSize == sizeof(CalcPacket)) {
    CalcPacket p;
    _udp.read((uint8_t*)&p, sizeof(CalcPacket));

    if (p.header == 0xAA && p.checksum == calculateChecksum(p)) {
      processCalculation(p, _udp.remoteIP(), _udp.remotePort());
    } else {
      Serial.println("Invalid Packet (Bad Header or Checksum)");
    }
  } else if (packetSize > 0) {
    Serial.printf("Received unexpected packet size: %d\n", packetSize);
  }
}

uint8_t UdpCalculator::calculateChecksum(const CalcPacket& p) {
  uint8_t* ptr = (uint8_t*)&p;
  uint8_t sum = 0;
  for (size_t i = 0; i < sizeof(CalcPacket) - 1; i++) {
    sum += ptr[i];
  }
  return sum;
}

uint8_t UdpCalculator::calculateResChecksum(const ResPacket& p) {
  uint8_t* ptr = (uint8_t*)&p;
  uint8_t sum = 0;
  for (size_t i = 0; i < sizeof(ResPacket) - 1; i++) {
    sum += ptr[i];
  }
  return sum;
}

void UdpCalculator::processCalculation(const CalcPacket& p, IPAddress remoteIP, uint16_t remotePort) {
  float result = 0;
  switch (p.op) {
    case 1: result = p.val1 + p.val2; break;
    case 2: result = p.val1 * p.val2; break;
    case 3: result = p.val1 - p.val2; break;
  }

  // Send Response back for latency measurement
  ResPacket res;
  res.header = 0xBB;
  res.result = result;
  res.checksum = calculateResChecksum(res);

  _udp.beginPacket(remoteIP, remotePort);
  _udp.write((uint8_t*)&res, sizeof(ResPacket));
  _udp.endPacket();

  // Blink LED to indicate success
  digitalWrite(ledPin, HIGH);
  delay(1); // Very short blink to not block much
  digitalWrite(ledPin, LOW);
}
