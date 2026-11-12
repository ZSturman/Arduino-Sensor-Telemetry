#pragma once

#include <IRremote.hpp>

#include "../core/SensorModule.h"

class IrReceiverSensor : public SensorModule {
 public:
  IrReceiverSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  String lastCodeHex_;
  unsigned long lastReadMs_ = 0;
};

