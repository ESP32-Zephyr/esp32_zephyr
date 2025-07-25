# ESP32 Zephyr

## Build

```console
$ west build -b esp32_devkitc/esp32/procpu -- -DSSID="WIFI_SSID" -DPSK="WIFI_PASS"
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
[00:00:02.244,000] <inf> wifi: Connecting to: WIFI_SSID
[00:00:02.246,000] <inf> wifi: Waiting for authorization...
[00:00:08.984,000] <dbg> esp32_wifi: esp32_wifi_send: pkt sent 0x3fff82c4 len 291
[00:00:11.090,000] <dbg> esp32_wifi: esp32_wifi_send: pkt sent 0x3fff82c4 len 298
[00:00:11.119,000] <inf> net_dhcpv4: Received: xxx.xxx.xxx.xxx
[00:00:15.939,000] <inf> wifi: Connected
[00:00:15.940,000] <inf> wifi: Wifi network state:
[00:00:15.940,000] <inf> wifi:  SSID: WIFI_SSID
[00:00:15.940,000] <inf> wifi:  Band: 2.4GHz
[00:00:15.940,000] <inf> wifi:  Channel: xx
[00:00:15.940,000] <inf> wifi:  Security: WPA2-PSK
[00:00:15.940,000] <inf> wifi:  RSSI: xx
[00:00:15.940,000] <inf> server: Creating new server object
[00:00:11.904,000] <inf> server: Waiting for TCP connections on port 4242...
```

## Test

### Service discovery

```console
$  avahi-resolve -4 -n esp32.local
esp32.local xxx.xxx.xxx.xxx
```

```console
$ avahi-browse -t -r _esp32._tcp
+ wlp4s0 IPv4 esp32                                         _esp32._tcp          local
= wlp4s0 IPv4 esp32                                         _esp32._tcp          local
   hostname = [esp32.local]
   address = [xxx.xxx.xxx.xxx]
   port = [4242]
   txt = []
```

### Send/Receive test message

```console
$ cd test
$ python3 esp32_client.py --dest_addr tcp:xxx.xxx.xxx.xxx:4242
```
