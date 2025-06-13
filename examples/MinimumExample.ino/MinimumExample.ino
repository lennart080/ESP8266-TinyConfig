#include <TinyConfig.h>

TinyConfig config;

void setup() {
    Serial.begin(115200);

    // Initialize TinyConfig
    config.begin();

    // Set a configuration value
    config.set("wifi_ssid", "MyWiFiNetwork");

    // Retrieve and print configuration values
    String ssid = config.get("wifi_ssid", "default_ssid");
    Serial.println("Configured WiFi SSID: " + ssid);

    // stop TinyConfig if you no longer need it
    config.StopTC();
}

void loop() {
    // Nothing to do
}