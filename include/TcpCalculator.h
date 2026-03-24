#ifndef TCP_CALCULATOR_H
#define TCP_CALCULATOR_H

#include <Arduino.h>
#include <WiFi.h>
#include "UdpCalculator.h" // Reuse CalcPacket and ResPacket

class TcpCalculator {
public:
  TcpCalculator(unsigned int port);
  void begin();
  void handleClients();

private:
  WiFiServer _server;
  unsigned int _port;
  void processClient(WiFiClient& client);
  uint8_t calculateChecksum(const CalcPacket& p);
  uint8_t calculateResChecksum(const ResPacket& p);
};

#endif // TCP_CALCULATOR_H
