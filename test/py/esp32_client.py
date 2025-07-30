#!/usr/bin/python3

# --- Imports --- #
import argparse
import socket

import cmds_pb2

class Esp32Ctl:
    """docstring for Esp32Ctl"""
    def __init__(self):
        super(Esp32Ctl, self).__init__()
        self.sock = None

    def cmd_send(self,  req: object, req_id: int):
        res = cmds_pb2.response()

        req.hdr.id = req_id
        req_raw = req.SerializeToString()
        print(f"--->\n{req}")
        self.sock.send(req_raw)
        res_raw = self.sock.recv()
        try:
            res.ParseFromString(res_raw)
            print(f"<---\n{res}")
        except Exception as err:
            print(f"Error parsing response: {err}")
            return None

        if res.hdr.ret != cmds_pb2.OK:
            print(f"Command failed! (ret: {res.hdr.ret}) {res.hdr.err_msg}")

        return res

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

    esp32_ctl = Esp32Ctl()
    if proto == "udp":
        esp32_ctl.sock = Esp32UDPSock(esp32_addr)
    elif proto == "tcp":
        esp32_ctl.sock = Esp32TCPSock(esp32_addr)
    else:
        print(f"Invalid protocol type: {proto}. Available protocols [ tcp | udp ]")
        return

    req = cmds_pb2.request()
    # Read Version
    esp32_ctl.cmd_send(req, cmds_pb2.VERSION_GET)
    # Get number of ADC channels
    res = esp32_ctl.cmd_send(req, cmds_pb2.ADC_CHS_GET)
    # Read ADC channel
    adc_chs = res.adc_chs_get.adc_chs
    for ch in range(adc_chs):
        req.adc_ch_read.ch = ch
        esp32_ctl.cmd_send(req, cmds_pb2.ADC_CH_READ)

    # Send invalid channel
    req.adc_ch_read.ch = adc_chs + 1
    esp32_ctl.cmd_send(req, cmds_pb2.ADC_CH_READ)

    # Configure PWM
    res = esp32_ctl.cmd_send(req, cmds_pb2.PWM_PERIOD_INTERVAL_GET)
    period = res.pwm_periods_get.period_max
    res = esp32_ctl.cmd_send(req, cmds_pb2.PWM_CHS_GET)
    pwm_chs = res.pwm_chs_get.pwm_chs
    for ch in range(pwm_chs):
        req.pwm_ch_set.ch = ch
        req.pwm_ch_set.period = period
        req.pwm_ch_set.pulse = period // 2 # 50% Duty Cycle
        esp32_ctl.cmd_send(req, cmds_pb2.PWM_CH_SET)
        req.pwm_ch_get.ch = ch
        esp32_ctl.cmd_send(req, cmds_pb2.PWM_CH_GET)


if __name__ == "__main__":
    main()
