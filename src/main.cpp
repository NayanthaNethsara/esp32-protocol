#include <Arduino.h>
#include <WiFi.h>

#include <WiFiUdp.h>

// This structure is our "Contract"
struct __attribute__((packed)) LayerPacket {
  uint8_t  header = 0x02;
  uint16_t taskId;
  float    h1;
  float    h2;
  uint8_t  checksum;
};

WiFiUDP udp;
const char* peerB_IP = "192.168.1.5"; // Set this to your peer's IP
const unsigned int port = 12345;       // Set this to your desired port

// Function to send binary data
void sendHiddenData(uint16_t id, float val1, float val2) {
  LayerPacket packet;
  packet.taskId = id;
  packet.h1 = val1;
  packet.h2 = val2;
  packet.checksum = (uint8_t)(val1 + val2); // Simple checksum logic

  udp.beginPacket(peerB_IP, port);
  udp.write((uint8_t*)&packet, sizeof(packet)); // Send raw bytes
  udp.endPacket();
}

const char *ssid = "SLT-Fiber-2.4G";
const char *password = "HyZG7257";

const char *hostIP = "192.168.1.4";
const uint16_t hostPort = 8080;

WiFiClient client;
const int ledPin = 2;

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  udp.begin(port); // Start UDP on the given port

  Serial.println("\nConnecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
}

void loop()
{
  // If we aren't connected to the Mac's Python server, try to connect
  if (!client.connected())
  {
    Serial.println("Attempting to connect to Mac Brain...");
    if (!client.connect(hostIP, hostPort))
    {
      Serial.println("Connection failed. Retrying in 2 seconds...");
      delay(2000);
      return;
    }
    Serial.println("Connected to Mac Brain!");
  }

  // 1. Generate the sensor data
  int fakeSensorValue = random(0, 100);

  // 2. Send the data over the Wi-Fi socket
  client.println(fakeSensorValue);

  // Example: Send binary packet over UDP as well
  sendHiddenData(1, (float)fakeSensorValue, (float)fakeSensorValue / 2.0f);

  // 3. Wait briefly for the Mac to send a decision back
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 500)
    {
      Serial.println("Timeout waiting for AI response.");
      return;
    }
  }

  // 4. Read the command and control the LED
  char command = client.read();
  if (command == '1')
  {
    digitalWrite(ledPin, HIGH);
  }
  else if (command == '0')
  {
    digitalWrite(ledPin, LOW);
  }

  // Stream data 10 times a second
  delay(100);
}