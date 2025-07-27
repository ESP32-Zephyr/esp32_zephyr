#!/usr/bin/python3

# --- Imports --- #
import argparse
import socket

import cmds_pb2

class Esp32TCPSock:
    """docstring for Esp32TCPSock"""
    def __init__(self, dest_addr: tuple):
        super(Esp32TCPSock, self).__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(5.0)
        self.sock.connect(dest_addr)

    def __dell__(self):
        self.sock.close()

    def send(self, msg: bytes):
        self.sock.send(msg)

    def recv(self) -> str:
        res = ""

        try:
            res = self.sock.recv(1024)
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

    def send(self, msg: bytes):
        self.sock.sendto(msg, self.dest_addr)

    def recv(self) -> str:
        res = ""

        try:
            res, _ = self.sock.recvfrom(1024)
        except socket.timeout:
            print("---| Receive timeout")

        return res

# --- Main entry point --- #
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dest_addr", help="ESP32 IPv4 address & port. proto:ipv4:port",
        type=str, required=True)
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

    req = cmds_pb2.request()
    res = cmds_pb2.response()

    # Read Version
    req.hdr.id = cmds_pb2.VERSION_GET
    req_raw = req.SerializeToString()
    print(f"---> Sending: \n{req}")
    esp32_sock.send(req_raw)
    res_raw = esp32_sock.recv()
    try:
        res.ParseFromString(res_raw)
        print(f"<--- Received: \n{res}")
    except Exception as err:
        print(str(err))

    # Get number of ADC channels
    req.hdr.id = cmds_pb2.ADC_CHS_GET
    req_raw = req.SerializeToString()
    print(f"---> Sending: \n{req}")
    esp32_sock.send(req_raw)
    res_raw = esp32_sock.recv()
    try:
        res.ParseFromString(res_raw)
        print(f"<--- Received: \n{res}")
    except Exception as err:
        print(str(err))

    # Read ADC values
    adc_chs = res.adc_chs_get.adc_chs
    for ch in range(adc_chs):
        req.hdr.id = cmds_pb2.ADC_CH_READ
        req.adc_ch_read.ch = ch
        req_raw = req.SerializeToString()
        print(f"---> Sending: \n{req}")
        esp32_sock.send(req_raw)
        res_raw = esp32_sock.recv()
        try:
            res.ParseFromString(res_raw)
            print(f"<--- Received: \n{res}")
        except Exception as err:
            print(str(err))

if __name__ == "__main__":
    main()
