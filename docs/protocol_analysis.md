# Protocol Analysis: Binary vs. Text (UDP/TCP)

This document breaks down the four communication methods implemented in this project and explains why their performance differs.

## 1. Binary UDP (Our Custom Protocol)
*   **Mechanism**: Data is packed into a raw 11-byte structure.
*   **Payload**: `[Header(1), Op(1), Val1(4), Val2(4), Checksum(1)]`
*   **Why it's Fast**: 
    *   **No Connection Overhead**: UDP is stateless; you just send the packet.
    *   **Fixed Size**: The ESP32 knows exactly how many bytes to read immediately.
    *   **Direct Mapping**: The bytes are copied directly into a C-struct using `memcpy` or pointer casting, requiring zero parsing or string-to-float conversion.

## 2. Binary TCP
*   **Mechanism**: The same raw 11-byte structure sent over a TCP stream.
*   **Why it's Slower**:
    *   **Handshake**: Every request requires a "Three-way handshake" (SYN -> SYN-ACK -> ACK) to establish a connection.
    *   **Flow Control**: TCP ensures data is received in order and retransmits lost packets, adding CPU and network overhead.
    *   **Teardown**: Closing the connection also requires a handshake (FIN -> ACK).

## 3. Typical (Text) UDP
*   **Mechanism**: Data is encoded as a string like `"1,10.0,20.0"`.
*   **Why it's Slower than Binary UDP**:
    *   **Variable Payload**: The size depends on the number of digits (e.g., `10.0` is 4 bytes vs `10.000` which is 6 bytes).
    *   **Parsing Overhead**: The ESP32 must scan the string for commas, split it, and then run `toFloat()` or `atof()`, which are computationally expensive on a micro-controller.

## 4. Typical (Text) TCP
*   **Mechanism**: Comma-separated strings sent over a TCP stream (most similar to standard HTTP/REST).
*   **Why it's the Slowest**:
    *   Inherits **all the overhead of TCP** (connection management).
    *   Inherits **all the overhead of Text Parsing** (string conversions).
    *   **Jitter**: The combination of network retransmissions and unpredictable parsing time leads to higher variability in response times.
