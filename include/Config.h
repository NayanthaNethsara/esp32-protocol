#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Credentials
extern const char* ssid;
extern const char* password;

// UDP Configuration
extern const unsigned int localPort;
extern const int ledPin;

#endif // CONFIG_H
