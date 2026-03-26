#include "Config.h"

// WiFi Credentials
const char *ssid = "SLT-Fiber-2.4G";
const char *password = "HyZG7257";
const int ledPin = 2; // Built-in LED on most ESP32 boards

// P2P Configuration
const int NODE_ID = 1;
const char *PEER_IP = "192.168.1.3"; // IP of your second ESP32
const unsigned int p2pPort = 12349;
