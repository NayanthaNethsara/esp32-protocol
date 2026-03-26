import socket
import struct
import time
import statistics

# Configuration
ESP32_NODE1_IP = "192.168.1.6" # IP of ESP32 acting as Node 1
P2P_PORT = 12349
NUM_TESTS = 50

def calculate_checksum(data):
    return sum(data) & 0xFF

def test_neural_flow():
    latencies = []
    print(f"Testing Distributed Neural Flow Latency (PC -> ESP1 -> ESP2 -> PC)...")
    
    # Create UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(2.0)
    
    # Activation Packet Header: 0xCC, Step: 0
    header = 0xCC
    step = 0
    val = 1.0 # Initial input x
    pc_ip = 0 # ESP1 will fill this using remoteIP()
    
    for seq in range(1, NUM_TESTS + 1):
        # Pack with sequence number
        # fmt: Header(B), Step(B), Seq(H), Port(H), Val(f), PC_IP(I)
        fmt = '<BBHHfI'
        pc_port = 0 # ESP1 will fill this
        partial_pkt = struct.pack(fmt, header, step, seq, pc_port, val, pc_ip)
        checksum = calculate_checksum(partial_pkt)
        packet = partial_pkt + struct.pack('<B', checksum)
        
        start = time.perf_counter()
        sock.sendto(packet, (ESP32_NODE1_IP, P2P_PORT))
        
        try:
            data, addr = sock.recvfrom(1024)
            end = time.perf_counter()
            
            # Unpack response
            # Format: B, B, H, H, f, I, B
            res_header, res_step, res_seq, res_port, res_val, res_pc_ip, res_checksum = struct.unpack('<BBHHfIB', data)
            
            if res_header == 0xCC and res_step == 2:
                if res_seq == seq:
                    latencies.append((end - start) * 1000)
                    if seq == 1:
                        print(f"First Result [Seq 1]: {res_val:.4f} (from {addr[0]})")
                else:
                    print(f"Sequence Mismatch! Sent {seq}, Got {res_seq}")
            else:
                print(f"Unexpected response step: {res_step}")
                
        except socket.timeout:
            print(f"Test {seq}: Timeout!")
            
    sock.close()
    return latencies

if __name__ == "__main__":
    results = test_neural_flow()
    
    if results:
        print("\n" + "="*50)
        print(f"Distributed Neural Flow Results ({len(results)} samples)")
        print("-" * 50)
        print(f"Average Latency: {statistics.mean(results):.3f} ms")
        print(f"Min Latency:     {min(results):.3f} ms")
        print(f"Max Latency:     {max(results):.3f} ms")
        print(f"Std Dev (Jitter): {statistics.stdev(results):.3f} ms")
        print("="*50)
        print("\nNote: This includes two network hops (PC->ESP1 and ESP1->ESP2) plus one final jump (ESP2->PC).")
    else:
        print("No results collected. Check your ESP32 connections and Node IDs.")
