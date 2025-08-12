import socket

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 1883       # MQTT default port (non-TLS)

def run_broker():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        print(f"Listening on {HOST}:{PORT}")

        while True:
            conn, addr = s.accept()
            with conn:
                print(f"Connected by {addr}")

                # Read fixed header (2 bytes)
                header = conn.recv(2)
                if len(header) < 2:
                    print("Incomplete header, closing connection")
                    continue

                packet_type = header[0] >> 4
                remaining_length = header[1]

                print(f"Packet type: {packet_type}, Remaining length: {remaining_length}")

                # Read the rest of the packet
                payload = b""
                while len(payload) < remaining_length:
                    chunk = conn.recv(remaining_length - len(payload))
                    if not chunk:
                        break
                    payload += chunk

                print(f"Received CONNECT packet, total length: {2 + len(payload)} bytes")

                # Send minimal CONNACK packet (success)
                connack = bytes([0x20, 0x02, 0x00, 0x00])
                conn.sendall(connack)
                print("Sent CONNACK packet")

                # Close the connection after responding
                print("Closing connection\n")

if __name__ == "__main__":
    run_broker()

