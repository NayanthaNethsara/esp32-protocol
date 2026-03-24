import socket
import struct
import time
import statistics

# Ports from localPort in main.cpp
BINARY_UDP_PORT = 12345
BINARY_TCP_PORT = 12346
TEXT_UDP_PORT   = 12347
TEXT_TCP_PORT   = 12348
ESP32_IP = "192.168.1.7"

NUM_TESTS = 100

def calculate_checksum(data):
    return sum(data) & 0xFF

def test_binary_udp():
    latencies = []
    print(f"Testing Binary UDP Latency ({NUM_TESTS} packets)...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(1.0)
    
    packet = struct.pack('<BBff', 0xAA, 1, 10.0, 20.0)
    packet += struct.pack('<B', calculate_checksum(packet))
    
    for _ in range(NUM_TESTS):
        start = time.perf_counter()
        sock.sendto(packet, (ESP32_IP, BINARY_UDP_PORT))
        try:
            sock.recvfrom(1024)
            latencies.append((time.perf_counter() - start) * 1000)
        except socket.timeout: pass
    sock.close()
    return latencies

def test_binary_tcp():
    latencies = []
    print(f"Testing Binary TCP Latency ({NUM_TESTS} requests)...")
    packet = struct.pack('<BBff', 0xAA, 1, 10.0, 20.0)
    packet += struct.pack('<B', calculate_checksum(packet))
    
    for _ in range(NUM_TESTS):
        start = time.perf_counter()
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(0.5)
            sock.connect((ESP32_IP, BINARY_TCP_PORT))
            sock.sendall(packet)
            sock.recv(1024)
            latencies.append((time.perf_counter() - start) * 1000)
            sock.close()
        except Exception: pass
    return latencies

def test_text_udp():
    latencies = []
    print(f"Testing Typical (Text) UDP Latency ({NUM_TESTS} packets)...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(1.0)
    
    request = "1,10.0,20.0"
    
    for _ in range(NUM_TESTS):
        start = time.perf_counter()
        sock.sendto(request.encode(), (ESP32_IP, TEXT_UDP_PORT))
        try:
            sock.recvfrom(1024)
            latencies.append((time.perf_counter() - start) * 1000)
        except socket.timeout: pass
    sock.close()
    return latencies

def test_text_tcp():
    latencies = []
    print(f"Testing Typical (Text) TCP Latency ({NUM_TESTS} requests)...")
    request = "1,10.0,20.0\n"
    
    for _ in range(NUM_TESTS):
        start = time.perf_counter()
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(0.5)
            sock.connect((ESP32_IP, TEXT_TCP_PORT))
            sock.sendall(request.encode())
            sock.recv(1024)
            latencies.append((time.perf_counter() - start) * 1000)
            sock.close()
        except Exception: pass
    return latencies

def print_stats(name, latencies):
    if not latencies:
        print(f"{name}: No data")
        return
    avg = statistics.mean(latencies)
    print(f"| {name:<12} | {avg:>8.3f} ms | {min(latencies):>8.3f} ms | {max(latencies):>8.3f} ms | {statistics.stdev(latencies):>8.3f} ms |")

if __name__ == "__main__":
    b_udp = test_binary_udp()
    b_tcp = test_binary_tcp()
    t_udp = test_text_udp()
    t_tcp = test_text_tcp()
    
    print("\n" + "="*70)
    print(f"{'Protocol':<12} | {'Average':>11} | {'Min':>11} | {'Max':>11} | {'Std Dev':>11}")
    print("-" * 70)
    print_stats("Binary UDP", b_udp)
    print_stats("Binary TCP", b_tcp)
    print_stats("Typical UDP", t_udp)
    print_stats("Typical TCP", t_tcp)
    print("="*70)
