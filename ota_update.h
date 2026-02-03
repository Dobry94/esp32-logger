inline void checkForUpdate() {
    HTTPClient http;

    Serial.println("[OTA] Checking for update...");
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

    if (newVersion == VERSION) {
        Serial.println("[OTA] No update needed");
        return;
    }

    Serial.printf("[OTA] Update available: %s\n", newVersion.c_str());
    Serial.println("[OTA] Downloading firmware...");

    // --- DOWNLOAD ---
    http.begin(FIRMWARE_URL);
    code = http.GET();
    if (code != 200) {
        Serial.println("[OTA] Firmware download failed");
        http.end();
        return;
    }

    int total = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (!Update.begin(total)) {
        Serial.println("[OTA] Update.begin failed");
        http.end();
        return;
    }

    uint8_t buff[1024];
    int downloaded = 0;
    int lastPercent = -1;

    // --- DOWNLOADING WITH PROGRESS ---
    while (http.connected() && (downloaded < total)) {
        size_t size = stream->available();
        if (size) {
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            downloaded += c;

            int percent = (downloaded * 100) / total;
            if (percent != lastPercent) {
                Serial.printf("[OTA] Downloading: %d%%\n", percent);
                lastPercent = percent;
            }

            // --- INSTALLING WITH PROGRESS ---
            size_t written = Update.write(buff, c);
            int installPercent = (Update.progress() * 100) / total;
            Serial.printf("[OTA] Installing: %d%%\n", installPercent);

            if (written != c) {
                Serial.println("[OTA] Write error");
                http.end();
                return;
            }
        }
        delay(1);
    }

    http.end();

    if (!Update.end()) {
        Serial.println("[OTA] Update.end failed");
        return;
    }

    if (!Update.isFinished()) {
        Serial.println("[OTA] Update not finished");
        return;
    }

    Serial.println("[OTA] Update OK");
    Serial.println("[OTA] Restarting...");
    delay(1000);
    ESP.restart();
}
