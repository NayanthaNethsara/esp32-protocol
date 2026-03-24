#ifndef UDP_CALCULATOR_H
#define UDP_CALCULATOR_H

#include <Arduino.h>
#include <WiFiUdp.h>

// Struct mapping the "Custom Protocol"
struct __attribute__((packed)) CalcPacket {
  uint8_t  header;    // 0xAA
  uint8_t  op;        // 1: Add, 2: Mul, 3: Sub
  float    val1;
  float    val2;
  uint8_t  checksum;  // Simple summation of bytes before checksum
};

struct __attribute__((packed)) ResPacket {
  uint8_t  header;    // 0xBB
  float    result;
  uint8_t  checksum;
};

class UdpCalculator {
public:
  UdpCalculator(unsigned int port);
  void begin();
  void handlePackets();

private:
  WiFiUDP _udp;
  unsigned int _port;
  uint8_t calculateChecksum(const CalcPacket& p);
  uint8_t calculateResChecksum(const ResPacket& p);
  void processCalculation(const CalcPacket& p, IPAddress remoteIP, uint16_t remotePort);
};

#endif // UDP_CALCULATOR_H
