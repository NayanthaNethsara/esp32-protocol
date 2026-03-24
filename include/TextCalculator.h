#ifndef TEXT_CALCULATOR_H
#define TEXT_CALCULATOR_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFi.h>

class TextCalculator {
public:
  TextCalculator(unsigned int udpPort, unsigned int tcpPort);
  void begin();
  void handleUdp();
  void handleTcp();

private:
  WiFiUDP _udp;
  WiFiServer _server;
  unsigned int _udpPort;
  unsigned int _tcpPort;
  
  String processRequest(String request);
};

#endif // TEXT_CALCULATOR_H
