#pragma once

#include "../core/SensorModule.h"

class RainSensor : public SensorModule {
 public:
  RainSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  int rainRaw_ = 0;
  bool rainDetected_ = false;
  unsigned long lastReadMs_ = 0;
};

