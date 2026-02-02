#include "ota_update.h"

void setup() {
    initSystem();
}

void loop() {
    handlePeriodicUpdate();
}
