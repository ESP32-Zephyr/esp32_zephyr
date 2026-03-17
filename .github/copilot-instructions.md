# Copilot Instructions for ESP32 Zephyr Project

## Project Overview
- **Target:** ESP32 running Zephyr RTOS
- **Major Components:**
  - `src/`: Main application logic (WiFi, server, ADC, PWM, NVS, command interpreter)
  - `proto/`: Protocol buffer definitions and generated code
  - `boards/`: Board-specific configuration and overlays
  - `modules/`: Zephyr and third-party modules
  - `doc/`: Documentation

## Build & Flash Workflow
- **Environment:** Source Zephyr environment before building:
  ```sh
  source [PATH_TO_ZEPHYRPORJECT]/zephyrproject/zephyr/zephyr-env.sh
  ```
- **Build:**
  ```sh
  west build -b esp32_devkitc/esp32/procpu
  ```
- **Flash:**
  ```sh
  west flash
  ```
- **Monitor UART:**
  ```sh
  west espressif monitor
  ```

## WiFi Setup
- Set credentials via UART shell after boot:
  ```sh
  nvs --ssid WIFI_SSID --pass WIFI_PASS
  ```
- Reboot device to connect.

## Testing
- **Service Discovery:**
  ```sh
  avahi-resolve -4 -n esp32.local
  ```
- **Client Test:**
  ```sh
  cd test/py
  python3 esp32_client.py --dest_addr tcp:xxx.xxx.xxx.xxx:4242
  ```

## Key Patterns & Conventions
- **Component Boundaries:** Each hardware feature (WiFi, ADC, PWM, NVS, server) is implemented as a separate C module in `src/` with matching `.c` and `.h` files.
- **Command Interpreter:** Central command handling in `cmd_interpreter.c`.
- **Protobuf:** Protocols defined in `proto/cmds.proto`, generated code in `proto/cmds.pb.[ch]`.
- **Configuration:** Board and app config in `boards/` and root `prj.conf`.
- **Logging:** Zephyr logging macros used for runtime diagnostics.

## Integration Points
- **Zephyr RTOS:** Uses Zephyr APIs for hardware, networking, and shell.
- **Espressif Tools:** Relies on `west` for build/flash/monitor.
- **Python Client:** Example client in `test/py/esp32_client.py` for TCP server testing.

## Tips for AI Agents
- Reference `README.md` for up-to-date workflow and logs.
- Follow the modular structure in `src/` for new features.
- Use Zephyr APIs and patterns for device interaction.
- Update `proto/` and regenerate code if protocol changes.
- Keep board-specific changes in `boards/` overlays/configs.
