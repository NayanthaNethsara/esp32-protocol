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
      processCalculation(p);
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

void UdpCalculator::processCalculation(const CalcPacket& p) {
  Serial.println("--- Packet Received ---");
  
  float result = 0;
  String opName = "";

  switch (p.op) {
    case 1: // Add
      result = p.val1 + p.val2;
      opName = "Addition";
      break;
    case 2: // Multiple
      result = p.val1 * p.val2;
      opName = "Multiplication";
      break;
    case 3: // Subtract
      result = p.val1 - p.val2;
      opName = "Subtraction";
      break;
    default:
      Serial.println("Unknown Operation!");
      return;
  }

  Serial.printf("Op: %s\n", opName.c_str());
  Serial.printf("Value 1: %.2f\n", p.val1);
  Serial.printf("Value 2: %.2f\n", p.val2);
  Serial.printf("Result: %.2f\n", result);
  Serial.println("-----------------------");

  // Blink LED to indicate success
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
}
