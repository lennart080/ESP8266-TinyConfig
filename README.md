# TinyConfig
[![License](https://img.shields.io/github/license/lennart080/ESP8266-TinyConfig?color=blue)](https://github.com/lennart080/ESP8266-TinyConfig/blob/main/LICENSE)
[![PlatformIO Registry](https://badgen.net/badge/PlatformIO/Available/green?icon=platformio)](https://registry.platformio.org/libraries/lennart080/ESP8266-TinyConfig)
[![GitHub Release](https://img.shields.io/github/v/release/lennart080/ESP8266-TinyConfig)](https://github.com/lennart080/ESP8266-TinyConfig/releases)

A lightweight configuration library for ESP8266 (Arduino/PlatformIO).  
Easily store and retrieve configuration data (such as WiFi credentials, settings, etc.) in a JSON file on the ESP8266's filesystem.

---

## Features

- **Simple API:** Store and retrieve integers, floats, and strings with ease.
- **JSON-based:** Configuration is stored as a JSON file on LittleFS.
- **Automatic file creation/reset:** Handles missing or corrupt config files.
- **Configurable file size:** Prevents oversized config files.
- **Detailed error handling:** Get error codes and human-readable error messages.

---

## Getting Started

### Hardware Requirements

- ESP8266-based board (e.g., NodeMCU, Wemos D1 Mini)
- PlatformIO or Arduino IDE

---

## Installation

### PlatformIO

Add this library to your `platformio.ini`:
```
lib_deps =
    https://github.com/lennart080/ESP8266-TinyConfig.git
    bblanchon/ArduinoJson
```

Or install via the PlatformIO Library Manager.

### Arduino IDE

1. Download this repository as a ZIP file.
2. In Arduino IDE, go to **Sketch > Include Library > Add .ZIP Library...** and select the downloaded ZIP.
3. Install the required dependencies:
    - [ArduinoJson](https://arduinojson.org/)
    - LittleFS (included with ESP8266 core)

---

## Usage

#### 1. Include and Create

```cpp
#include <TinyConfig.h>
TinyConfig config;
```

#### 2. Initialize the Filesystem

Call `StartTC()` in your `setup()` function to mount the filesystem and prepare the config file.

```cpp
void setup() {
    Serial.begin(115200);

    if (!config.StartTC()) {
        Serial.println("Init failed: " + config.getLastErrorString());
        return;
    }
}
```

#### 4. Set Configuration Values

Store values (such as WiFi credentials or settings):

```cpp
config.set("wifi_ssid", "MyNetwork");
config.set("wifi_pass", "MyPassword");
config.set("boot_count", 1);
```

#### 5. Retrieve Configuration Values

Read values from the config file, providing a fallback if the key does not exist:

```cpp
String ssid = config.getString("wifi_ssid", "default_ssid");
int bootCount = config.getInt("boot_count", 0);

Serial.println("SSID: " + ssid);
Serial.print("Boot count: ");
Serial.println(bootCount);
```

#### 6. Reset Configuration (Optional)

To reset the configuration file to an empty JSON object:

```cpp
config.resetConfig();
```

#### 7. Unmount the Filesystem

When finished, unmount the filesystem:

```cpp
config.StopTC();
```

---

## API Reference

| Method                                             | Description                                      |
|----------------------------------------------------|--------------------------------------------------|
| `bool StartTC()`                                   | Mounts the filesystem and prepares the config file. |
| `bool StopTC()`                                    | Unmounts the filesystem.                         |
| `bool set(const String& key, int/float/String)`    | Set a value in the config.                       |
| `int getInt(const String& key, int fallback)`      | Get an integer value or fallback.                |
| `float getFloat(const String& key, float fallback)`| Get a float value or fallback.                   |
| `String getString(const String& key, String fallback)` | Get a string value or fallback.              |
| `bool resetConfig()`                               | Resets config to empty JSON.                     |
| `void setMaxFileSize(size_t maxSize)`              | Set max config file size in bytes.               |
| `TinyConfigError getLastError() const`             | Get the last error code.                         |
| `String getLastErrorString() const`                | Get a string describing the last error.          |

---

## Troubleshooting

- **LittleFS mount failed:** Ensure the filesystem is formatted and available.
- **File not found:** The config file will be created automatically if missing.
- **File too large:** Use `setMaxFileSize()` to increase the limit if needed.
- **Not initialized:** Call `StartTC()` before using other methods.

---

## License

This project is licensed under the Apache 2.0 License.

You may use, share and modify it, as long as you give credit to:

**Lennart Gutjahr (2025)** — Original author  
**gutjahrlennart@gmail.com** — Email

License details: [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

---

## Credits

Based on [ArduinoJson](https://arduinojson.org/) and [LittleFS](https://github.com/earlephilhower/arduino-esp8266littlefs).