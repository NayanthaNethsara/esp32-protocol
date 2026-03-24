import socket

# Listen on all available network interfaces on port 8080
HOST = '0.0.0.0' 
PORT = 8080

print(f"Starting AI Brain Server on port {PORT}...")

# Create a TCP/IP socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print("Waiting for ESP32 to connect over Wi-Fi...")
    
    # Accept the connection from the ESP32
    conn, addr = s.accept()
    with conn:
        print(f"SUCCESS! ESP32 connected from {addr}")
        
        while True:
            # Receive up to 1024 bytes of data
            data = conn.recv(1024)
            if not data:
                break # Connection closed by ESP32
            
            raw_data = data.decode('utf-8').strip()
            
            if raw_data.isdigit():
                sensor_value = int(raw_data)
                print(f"Received via Wi-Fi: {sensor_value}")
                
                # ---------------------------------------------------------
                # NEURAL NETWORK LOGIC HERE
                # ---------------------------------------------------------
                if sensor_value > 80:
                    print("AI says: Anomaly detected! Sending '1'")
                    conn.sendall(b'1')
                else:
                    conn.sendall(b'0')