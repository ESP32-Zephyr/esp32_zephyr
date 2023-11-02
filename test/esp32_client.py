#!/usr/bin/python3

# --- Imports --- #
import argparse
import socket

class Esp32TCPSock:
    """docstring for Esp32TCPSock"""
    def __init__(self, dest_addr: tuple):
        super(Esp32TCPSock, self).__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(5.0)
        self.sock.connect(dest_addr)

    def __dell__(self):
        self.sock.close()

    def send(self, msg: str):
        self.sock.send(msg.encode())

    def recv(self) -> str:
        res = ""

        try:
            res = self.sock.recv(1024)
            res = res.decode()
        except socket.timeout:
            print("---| Receive timeout")

        return res

class Esp32UDPSock:
    """docstring for Esp32UDPSock"""
    def __init__(self, dest_addr: tuple):
        super(Esp32UDPSock, self).__init__()
        self.dest_addr = dest_addr
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(5.0)

    def __dell__(self):
        self.sock.close()

    def send(self, msg: str):
        self.sock.sendto(msg.encode(), self.dest_addr)

    def recv(self) -> str:
        res = ""

        try:
            res, _ = self.sock.recvfrom(1024)
            res = res.decode()
        except socket.timeout:
            print("---| Receive timeout")

        return res

# --- Main entry point --- #
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dest_addr", help="ESP32 IPv4 address & port. proto:ipv4:port", type=str, required=True)
    parser.add_argument("--msg_cnt", help="Number of test messages to send",
        type=int, default=3)
    args = parser.parse_args()

    addr = args.dest_addr.split(":")
    proto = addr[0]
    esp32_addr = (addr[1], int(addr[2]))

    if proto == "udp":
        esp32_sock = Esp32UDPSock(esp32_addr)
    elif proto == "tcp":
        esp32_sock = Esp32TCPSock(esp32_addr)
    else:
        print(f"Invalid protocol type: {proto}. Available protocols [ tcp | udp ]")
        return

    for i in range(args.msg_cnt):
        test_msg = f"Test message: {i}"
        print(f"---> Sending: {test_msg}")
        esp32_sock.send(test_msg)
        test_msg_res = esp32_sock.recv()
        if test_msg_res != "":
            print(f"<--- Received: {test_msg_res}")


if __name__ == "__main__":
    main()
