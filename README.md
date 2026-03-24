# ESP32 UDP Custom Protocol Calculator

A low-latency UDP-based communication system between an ESP32 and a Mac/PC for performing remote calculations.

## Project Structure

```text
.
├── include/
│   ├── Config.h            # Configuration constants (WiFi, Ports)
│   └── UdpCalculator.h     # UDP Protocol and Calculator class definition
├── src/
│   ├── main.cpp            # Main entry point (Setup & Loop)
│   ├── Config.cpp          # Configuration implementation
│   └── UdpCalculator.cpp   # UDP Protocol implementation
├── python/
│   ├── brain.py             # AI/Decision-making script
│   ├── send_udp.py          # UDP Packet sender script
│   └── latency_test.py     # 4-way protocol benchmark
├── protocol_analysis.md     # [NEW] Detailed analysis of each protocol
├── platformio.ini           # PlatformIO configuration
└── README.md                # This file
```

## Features

- **Custom UDP Protocol**: Packed binary structures for minimal latency and maximum throughput.
- **Checksum Verification**: Ensures data integrity for every packet received.
- **Modular Design**: Separated configuration, protocol logic, and main application code.
- **Python Integration**: Easy-to-use Python scripts for sending commands and processing data.

## Getting Started

### 1. ESP32 Setup

1. Open the project in **PlatformIO**.
2. Edit `src/Config.cpp` to include your WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```
3. Upload the firmware to your ESP32.
4. Open the Serial Monitor (115200 baud) and note the **IP Address**.

### 2. Python Setup

1. Navigate to the `python/` directory.
2. Edit `send_udp.py` and set `ESP32_IP` to the address from Step 1.
3. Run the sender script:
   ```bash
   python3 send_udp.py
   ```

## Custom Protocol Specification

Packets are sent as raw bytes in the following format:

| Offset | Type    | Name      | Description                     |
| :----- | :------ | :-------- | :------------------------------ |
| 0      | uint8_t | Header    | 0xAA                            |
| 1      | uint8_t | Operation | 1: Add, 2: Mul, 3: Sub          |
| 2-5    | float   | Val 1     | First operand (32st-bit float)  |
| 6-9    | float   | Val 2     | Second operand (32st-bit float) |
| 10     | uint8_t | Checksum  | Sum of bytes 0-9 (mod 256)      |

## Latency Comparison (4-Way Benchmark)

The project includes a benchmarking script to compare 4 different communication methods:

1.  **Binary UDP**: Custom packed struct (Lowest latency).
2.  **Binary TCP**: Custom packed struct over TCP.
3.  **Typical (Text) UDP**: Comma-separated strings over UDP.
4.  **Typical (Text) TCP**: Comma-separated strings over TCP.

### How to Run the Benchmark

1. Ensure the ESP32 is running the latest firmware.
2. Run the latency test script:
   ```bash
   python3 python/latency_test.py
   ```

### Protocol Details

| Protocol    | Port  | Format                  |
| :---------- | :---- | :---------------------- |
| Binary UDP  | 12345 | Packed Struct           |
| Binary TCP  | 12346 | Packed Struct           |
| Typical UDP | 12347 | `OP,VAL1,VAL2` String   |
| Typical TCP | 12348 | `OP,VAL1,VAL2\n` String |

## Development

- **ESP32**: Built using Arduino framework on PlatformIO. Decomposed into modular components:
  - `UdpCalculator`: Handles custom UDP packets and responses.
  - `TcpCalculator`: Handles TCP client connections and calculation requests.
- **Python**: Benchmark script uses `perf_counter` for high-precision timing.
