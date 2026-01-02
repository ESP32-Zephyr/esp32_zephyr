# ESP32 Zephyr

## Build

```console
$ source  [PATH_TO_ZEPHYRPORJECT]/zephyrproject/zephyr/zephyr-env.sh
$ west build -b esp32_devkitc/esp32/procpu
```

## Flash

```console
$ west flash
```

## Set Wifi credentials

```console
$ west espressif monitor
```

After Zephyr has booted set the Wifi credentials using the `nvs` command:

```console
uart:~$ nvs --ssid WIFI_SSID --pass WIFI_PASS
```

where `WIFI_SSID` is the SSID of the Wifi network that you wish to connect to
and `WIFI_PASS` the authentication for it.

Reboot the device. The boot log should look similar to the one below:

```console
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3ffb0000,len:19828
load:0x40080000,len:68272
1150 mmu set 00010000, pos 00010000
entry 0x4008a4b4
I (52) soc_init: ESP Simple boot
I (52) soc_init: compile time Jan  2 2026 21:27:31
W (52) soc_init: Unicore bootloader
I (52) soc_init: chip revision: v3.1
I (55) flash_init: SPI Speed      : 40MHz
I (59) flash_init: SPI Mode       : DIO
I (63) flash_init: SPI Flash Size : 4MB
I (66) boot: DRAM   : lma=00001020h vma=3ffb0000h size=04d74h ( 19828)
I (72) boot: IRAM   : lma=00005d9ch vma=40080000h size=10ab0h ( 68272)
I (78) boot: IROM   : lma=00020000h vma=400d0000h size=7758ch (488844)
I (84) boot: DROM   : lma=000a0000h vma=3f400000h size=1b564h (111972)
I (102) boot: libc heap size 47 kB.
I (102) spi_flash: detected chip: generic
I (102) spi_flash: flash io: dio

[00:00:00.140,000] <inf> wifi_init: rx ba win: 6
*** Booting Zephyr OS build v4.3.0-3055-g14fb2c069113 ***
*** Application esp32_devkitc_wroom-v0.1.0-main-7db168f48ddf47348c2b8afed5b4302311b403e5 2026-01-02 16:47:55 +0200 ***
[00:00:00.153,000] <inf> fs_nvs: 3 Sectors of 4096 bytes
[00:00:00.153,000] <inf> fs_nvs: alloc wra: 0, ec0
[00:00:00.153,000] <inf> fs_nvs: data wra: 0, 140
[00:00:00.154,000] <inf> wifi: Connecting to: WIFI_SSID
[00:00:00.224,000] <inf> wifi: Waiting for authorization...
[00:00:01.567,000] <inf> wifi: Connected
Connected
[00:00:01.568,000] <inf> wifi: Wifi network state:
[00:00:01.569,000] <inf> wifi:  SSID: WIFI_SSID
[00:00:01.569,000] <inf> wifi:  Band: 2.4GHz
[00:00:01.569,000] <inf> wifi:  Channel: 6
[00:00:01.569,000] <inf> wifi:  Security: WPA2-PSK
[00:00:01.569,000] <inf> wifi:  RSSI: -77
[00:00:01.569,000] <inf> pwm: Initializing PWM channels...

[00:00:01.569,000] <inf> server: Creating new server object
[00:00:01.570,000] <inf> server: Waiting for TCP connections on port 4242...
```

## Test

### Service discovery

```console
$  avahi-resolve -4 -n esp32.local
esp32.local xxx.xxx.xxx.xxx
```

### Send/Receive test message

```console
$ cd test/py
$ python3 esp32_client.py --dest_addr tcp:xxx.xxx.xxx.xxx:4242
```
