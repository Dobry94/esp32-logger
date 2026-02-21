#include "ota_update.h"

void setup() {
    initSystem();
    pinMode(4,OUTPUT);
}

void loop() {
    handlePeriodicUpdate();
    digitalWrite(4, HIGH);
}
