#!/usr/bin/python3

# --- Imports --- #
import argparse
import socket

# --- Main entry point --- #
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dest_addr", help="ESP32 IPv4 address & port. ipv4:port", type=str, required=True)
    parser.add_argument("--msg_cnt", help="Number of test messages to send",
        type=int, default=3)
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5.0)
    addr = args.dest_addr.split(":")
    esp32_addr = (addr[0], int(addr[1]))

    for i in range(args.msg_cnt):
        test_msg = f"Test message: {i}"
        print(f"---> Sending: {test_msg}")
        sock.sendto(test_msg.encode(), esp32_addr)
        try:
            data, _ = sock.recvfrom(1024)
            print(f"<--- Received: {data.decode()}")
        except socket.timeout:
            print("---| Receive timeout")

if __name__ == "__main__":
    main()
