#pragma once

#include "../core/SensorModule.h"

class PushButtonSensor : public SensorModule {
 public:
  PushButtonSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  bool pressed_ = false;
  bool lastPublishedPressed_ = false;
  unsigned long lastReadMs_ = 0;
};

