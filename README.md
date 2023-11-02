# ESP32 Zephyr

## Build

```console
$ west build -b esp32_devkitc_wroom -- -DSSID="WIFI_SSID" -DPSK="WIFI_PASS"
```

where `WIFI_SSID` is the SSID of the Wifi network that you wish to connect to
and `WIFI_PASS` the authentication for it.

## Flash

```console
$ west flash
```

## Monitor

```console
$ west espressif monitor
```

The results should look similar to the output shown below:

```console
*** Booting Zephyr OS build zephyr-v3.5.0-660-ge7340282a795 ***
Connecting to: WIFI_SSID
Waiting for authorization...
[00:00:08.984,000] <dbg> esp32_wifi: esp32_wifi_send: pkt sent 0x3fff82c4 len 291
[00:00:11.090,000] <dbg> esp32_wifi: esp32_wifi_send: pkt sent 0x3fff82c4 len 298
[00:00:11.119,000] <inf> net_dhcpv4: Received: xxx.xxx.xxx.xxx
Connected
Wifi network state:
    SSID: WIFI_SSID
    Band: 2.4GHz
    Channel: xx
    Security: WPA2-PSK
    RSSI: xx
Disconnected
```
