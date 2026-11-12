#pragma once

#include <DHT.h>

#include "../core/SensorModule.h"

class DhtSensor : public SensorModule {
 public:
  DhtSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  DHT dht_;
  unsigned long lastReadMs_ = 0;
  float temperatureC_ = NAN;
  float humidityPct_ = NAN;
  bool healthy_ = false;
};

