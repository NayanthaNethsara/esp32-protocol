#include "ActivationService.h"
#include "Config.h"
#include <math.h>

ActivationService::ActivationService(unsigned int port) : _port(port) {}

void ActivationService::begin() {
  _udp.begin(_port);
  Serial.printf("Activation Service started on port %d (Node %d)\n", _port, NODE_ID);
}

void ActivationService::handlePackets() {
  int packetSize = _udp.parsePacket();
  if (packetSize >= (int)sizeof(ActivationPacket)) {
    ActivationPacket pkt;
    _udp.read((unsigned char*)&pkt, sizeof(ActivationPacket));
    
    // Verify header and checksum
    if (pkt.header != 0xCC) return;
    
    uint8_t receivedSum = pkt.checksum;
    pkt.checksum = 0;
    if (calculateChecksum(&pkt) != receivedSum) return;

    if (NODE_ID == 1 && pkt.step == 0) {
      processNode1(&pkt, _udp.remoteIP());
    } else if (NODE_ID == 2 && pkt.step == 1) {
      processNode2(&pkt);
    }
  }
}

void ActivationService::processNode1(ActivationPacket* pkt, IPAddress remoteIP) {
  Serial.printf("Node 1 [Seq:%u]: Processing input %.2f. Forwarding to %s:%u\n", 
                pkt->sequence, pkt->val, PEER_IP, p2pPort);
  
  // Layer 1: y = x * 0.5 + 0.1
  pkt->val = pkt->val * 0.5f + 0.1f;
  pkt->step = 1;
  pkt->pc_ip = (uint32_t)remoteIP;
  pkt->pc_port = _udp.remotePort();
  pkt->checksum = calculateChecksum(pkt);

  // Forward to Node 2
  if (_udp.beginPacket(PEER_IP, p2pPort)) {
    _udp.write((unsigned char*)pkt, sizeof(ActivationPacket));
    if (_udp.endPacket()) {
      Serial.println("Node 1: Forwarded successfully.");
    } else {
      Serial.println("Node 1: Forwarding FAILED (endPacket).");
    }
  } else {
    Serial.println("Node 1: Forwarding FAILED (beginPacket).");
  }
  
  // Blink
  digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(ledPin, LOW);
}

void ActivationService::processNode2(ActivationPacket* pkt) {
  Serial.printf("Node 2 [Seq:%u]: Received partial %.2f. Sending back to %s:%u\n", 
                pkt->sequence, pkt->val, IPAddress(pkt->pc_ip).toString().c_str(), pkt->pc_port);
  
  // Layer 2 + Activation: output = tanh(y * 0.8 - 0.2)
  float y = pkt->val * 0.8f - 0.2f;
  pkt->val = tanhf(y);
  pkt->step = 2; // Final
  pkt->checksum = calculateChecksum(pkt);

  // Send back to PC
  IPAddress pcIP(pkt->pc_ip);
  if (_udp.beginPacket(pcIP, pkt->pc_port)) {
    _udp.write((unsigned char*)pkt, sizeof(ActivationPacket));
    if (_udp.endPacket()) {
      Serial.println("Node 2: Response sent to PC.");
    } else {
      Serial.println("Node 2: Response FAILED (endPacket).");
    }
  } else {
    Serial.println("Node 2: Response FAILED (beginPacket).");
  }

  // Blink
  digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(ledPin, LOW);
}

uint8_t ActivationService::calculateChecksum(ActivationPacket* pkt) {
  uint8_t* p = (uint8_t*)pkt;
  uint8_t sum = 0;
  for (size_t i = 0; i < sizeof(ActivationPacket) - 1; i++) {
    sum += p[i];
  }
  return sum;
}
