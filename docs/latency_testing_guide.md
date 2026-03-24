# Latency Testing Methodology

This guide explains how we measure the **Round-Trip Time (RTT)** between your Mac and the ESP32.

## How we measure it
The `latency_test.py` script uses the following logic for each packet/request:

1.  **Start Timer**: We record the current time with microsecond precision using `time.perf_counter()`.
2.  **Send Request**: The packet (Binary or Text) is sent over the network.
3.  **Wait for Response**: The script blocks until it receives a response packet from the ESP32.
4.  **Stop Timer**: The time is recorded again immediately after the response arrives.
5.  **Calculate RTT**: `Latency = (Stop - Start) * 1000` (result in milliseconds).

## What the statistics mean
*   **Average**: The typical performance you can expect. 
*   **Min**: The "best-case" scenario, usually when the WiFi channel is clear and the ESP32 is idle.
*   **Max**: The "worst-case" scenario. Large spikes in UDP often indicate a lost packet or WiFi interference. Large spikes in TCP often indicate a retransmission timeout.
*   **Standard Deviation (Std Dev)**: A measure of **Jitter**. A low Std Dev means the latency is very predictable, which is crucial for real-time control (like a drone or game controller).

## Why its accurate
- We use `time.perf_counter()` which is the highest resolution clock available in Python for measuring durations.
- We run **100 tests** per protocol to smooth out occasional network glitches and provide a statistically significant average.
