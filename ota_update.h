inline void checkForUpdate() {
    HTTPClient http;

    Serial.println("[OTA] Checking for update...");

    // --- Pobieranie wersji ---
    http.begin(VERSION_URL);
    int code = http.GET();
    if (code != 200) {
        Serial.println("[OTA] Version check failed");
        http.end();
        return;
    }

    String newVersion = http.getString();
    newVersion.trim();
    http.end();

    Serial.printf("[OTA] Current: %s\n", VERSION);
    Serial.printf("[OTA] Available: %s\n", newVersion.c_str());

    if (newVersion == VERSION) {
        Serial.println("[OTA] No update needed");
        return;
    }

    Serial.println("[OTA] Update available!");
    Serial.println("[OTA] Downloading firmware...");

    // --- Pobieranie firmware ---
    http.begin(FIRMWARE_URL);
    code = http.GET();
    if (code != 200) {
        Serial.println("[OTA] Firmware download failed");
        http.end();
        return;
    }

    int len = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    Serial.println("[OTA] Installing...");

    // --- Instalacja ---
    if (!Update.begin(len)) {
        Serial.println("[OTA] Update.begin failed");
        http.end();
        return;
    }

    size_t written = Update.writeStream(*stream);
    if (written != len) {
        Serial.println("[OTA] Written size mismatch");
        http.end();
        return;
    }

    if (!Update.end()) {
        Serial.println("[OTA] Update.end failed");
        http.end();
        return;
    }

    if (!Update.isFinished()) {
        Serial.println("[OTA] Update not finished");
        http.end();
        return;
    }

    Serial.println("[OTA] Update OK");
    Serial.println("[OTA] Restarting...");
    http.end();
    delay(1000);
    ESP.restart();
}
