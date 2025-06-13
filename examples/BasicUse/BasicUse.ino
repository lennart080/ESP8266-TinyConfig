#include <TinyConfig.h>
#include <ESP8266WiFi.h>

TinyConfig config;

void setup() {
    Serial.begin(115200);

    // Initialize TinyConfig
    if (!config.StartTC()) {
        Serial.println("Failed to initialize TinyConfig! " + config.getLastErrorString());
        return;
    }

    // Get WiFi credentials from config, or set defaults if not present
    String ssid = config.get("wifi_ssid", "MyWiFiNetwork");
    String pass = config.get("wifi_pass", "MySecretPassword");

    WiFi.begin(ssid.c_str(), pass.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect to WiFi.");
    }
}

void loop() {
    // Nothing to do
}