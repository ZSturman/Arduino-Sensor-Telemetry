#pragma once

#include "../core/SensorModule.h"

class JoystickSensor : public SensorModule {
 public:
  JoystickSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  int x_ = 0;
  int y_ = 0;
  bool buttonPressed_ = false;
  bool lastPublishedButton_ = false;
  unsigned long lastReadMs_ = 0;
};

