#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Credentials
extern const char* ssid;
extern const char* password;
extern const int ledPin;

// P2P Configuration
extern const int NODE_ID;       // 1 or 2
extern const char* PEER_IP;     // IP of the other ESP32
extern const unsigned int p2pPort;

#endif // CONFIG_H
