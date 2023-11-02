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
[00:00:15.940,000] <inf> server: Listeing on UDP port 4242...
```

## Test

```console
$ cd test
$ go run esp32_client.go  --dest_addr xxx.xxx.xxx.xxx:4242
```

or

```console
$ cd test
$ python3 esp32_client.py --dest_addr xxx.xxx.xxx.xxx:4242
```

where `xxx.xxx.xxx.xxx` is the IPv4 address obtained from the DHCP. This address
is displayed in the log on the line:

```
<inf> net_dhcpv4: Received: xxx.xxx.xxx.xxx
```
