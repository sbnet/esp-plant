# esp-plant

Minimal ESP8266 plant monitor with OLED face rendering and a REST API.

## Quick setup

1. Install [PlatformIO](https://platformio.org/).
2. Create a local config file from [`platformio.local.example.ini`](platformio.local.example.ini):

   ```ini
   [env:nodemcuv2]
   build_flags =
     -I include
     -DWIFI_SSID=\"your_wifi_ssid\"
     -DWIFI_PASSWORD=\"your_wifi_password\"
   ```

   Save it as `platformio.local.ini` at the project root.
3. Build the firmware:

   ```bash
   pio run
   ```

4. Upload to board:

   ```bash
   pio run -t upload
   ```

5. Open serial monitor:

   ```bash
   pio device monitor
   ```

## REST endpoints

- `GET /status`
  - Returns current visual state, simulated readings, and active thresholds.
- `POST /config`
  - Updates threshold config.
  - JSON body example:

    ```json
    {
      "dryMoistureMax": 30,
      "goodMoistureMin": 65,
      "lowLightMax": 20
    }
    ```

## Notes

- Wi-Fi credentials are loaded from compile-time defines in [`WifiSecrets.h`](include/WifiSecrets.h).
- Local secrets file [`platformio.local.ini`](platformio.local.ini) is ignored by Git via [`.gitignore`](.gitignore).
- Threshold config is persisted in emulated EEPROM with validation and fallback defaults.
