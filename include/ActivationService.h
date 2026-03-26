#ifndef ACTIVATION_SERVICE_H
#define ACTIVATION_SERVICE_H

#include <Arduino.h>
#include <WiFiUdp.h>

struct ActivationPacket {
  uint8_t header;     // 0xCC
  uint8_t step;       // 0: From PC, 1: From Peer, 2: Final
  uint16_t sequence;  // Request sequence number
  uint16_t pc_port;   // Original sender Port
  float val;
  uint32_t pc_ip;     // Original sender IP
  uint8_t checksum;
} __attribute__((packed));

class ActivationService {
public:
  ActivationService(unsigned int port);
  void begin();
  void handlePackets();

private:
  WiFiUDP _udp;
  unsigned int _port;
  
  void processNode1(ActivationPacket* pkt, IPAddress remoteIP);
  void processNode2(ActivationPacket* pkt);
  uint8_t calculateChecksum(ActivationPacket* pkt);
};

#endif // ACTIVATION_SERVICE_H
