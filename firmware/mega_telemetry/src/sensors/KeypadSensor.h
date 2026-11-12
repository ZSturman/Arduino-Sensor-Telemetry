#pragma once

#include <Keypad.h>

#include "../core/SensorModule.h"

class KeypadSensor : public SensorModule {
 public:
  KeypadSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  char keymap_[4][4];
  byte rowPins_[4];
  byte columnPins_[4];
  Keypad* keypad_ = nullptr;
  String lastKey_;
  unsigned long lastReadMs_ = 0;
};

