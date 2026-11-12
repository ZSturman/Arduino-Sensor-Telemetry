#pragma once

#include "../core/SensorModule.h"

class PirSensor : public SensorModule {
 public:
  PirSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  bool motion_ = false;
  bool lastPublishedMotion_ = false;
  unsigned long lastReadMs_ = 0;
};

