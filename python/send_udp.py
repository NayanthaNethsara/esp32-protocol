import socket
import struct
import time

# ESP32 Configuration
ESP32_IP = "192.168.1.100"
ESP32_PORT = 12345

def calculate_checksum(data):
    """Simple summation checksum matching ESP32 logic."""
    return sum(data) & 0xFF

def send_packet(op, val1, val2):
    # Struct Format:
    # B: uint8_t (header)
    # B: uint8_t (op)
    # f: float (val1)
    # f: float (val2)
    # B: uint8_t (checksum)
    
    header = 0xAA
    # Pack without checksum first to calculate it
    partial_data = struct.pack('<BBff', header, op, val1, val2)
    checksum = calculate_checksum(partial_data)
    
    # Final packed data
    packet = partial_data + struct.pack('<B', checksum)
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(packet, (ESP32_IP, ESP32_PORT))
    print(f"Sent: Op={op}, Val1={val1}, Val2={val2}, Checksum={checksum:#04x}")
    sock.close()

if __name__ == "__main__":
    print(f"UDP Packet Sender (Target: {ESP32_IP}:{ESP32_PORT})")
    print("Commands: 1 (Add), 2 (Mul), 3 (Sub), q (Quit)")
    
    try:
        while True:
            cmd = input("\nEnter operation (1, 2, 3 or q): ").strip()
            if cmd.lower() == 'q':
                break
            
            if cmd not in ['1', '2', '3']:
                print("Invalid operation!")
                continue
                
            try:
                v1 = float(input("Value 1: "))
                v2 = float(input("Value 2: "))
                send_packet(int(cmd), v1, v2)
            except ValueError:
                print("Invalid numeric input!")
                
    except KeyboardInterrupt:
        print("\nExiting...")
