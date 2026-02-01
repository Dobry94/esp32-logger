#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* WIFI_SSID = "NETIASPOT-nTE2";
const char* WIFI_PASS = "eJYfChyPUs7aX";

const char* VERSION = "1.0.0";  // aktualna wersja wgrana na ESP32

const char* VERSION_URL = "https://raw.githubusercontent.com/Dobry94/esp32-logger/refs/heads/main/firmware/version.txt";
const char* FIRMWARE_URL = "https://raw.githubusercontent.com/Dobry94/esp32-logger/refs/heads/main/firmware/firmware.bin";

void checkForUpdate() {
  HTTPClient http;
  http.begin(VERSION_URL);
  int code = http.GET();
  if (code != 200) {
    Serial.println("Version check failed");
    http.end();
    return;
  }

  String newVersion = http.getString();
  newVersion.trim();
  http.end();
  Serial.print("Current: ");
  Serial.println(VERSION);
  Serial.print("Available: ");
  Serial.println(newVersion);

  if (newVersion == VERSION) {
    Serial.println("No update needed");
    return;
  }

  Serial.println("Updating...");

  http.begin(FIRMWARE_URL);
  code = http.GET();
  if (code != 200) {
    Serial.println("Firmware download failed");
    http.end();
    return;
  }

  int len = http.getSize();
  WiFiClient * stream = http.getStreamPtr();

  if (!Update.begin(len)) {
    Serial.println("Update.begin failed");
    http.end();
    return;
  }

  size_t written = Update.writeStream(*stream);
  if (written != len) {
    Serial.println("Written size mismatch");
    http.end();
    return;
  }

  if (!Update.end()) {
    Serial.println("Update.end failed");
    http.end();
    return;
  }

  if (!Update.isFinished()) {
    Serial.println("Update not finished");
    http.end();
    return;
  }

  Serial.println("Update OK, restarting...");
  http.end();
  delay(1000);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // na start możesz sprawdzać update:
  // checkForUpdate();
}

void loop() {
  // normalna logika
    static unsigned long last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    checkForUpdate();}
    Serial.println("Siemanko"); //test actions
}
