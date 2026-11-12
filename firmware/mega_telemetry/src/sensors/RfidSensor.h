#pragma once

#include <MFRC522.h>
#include <SPI.h>

#include "../core/SensorModule.h"

class RfidSensor : public SensorModule {
 public:
  RfidSensor();
  void begin(TelemetryPublisher& publisher) override;
  void tick(unsigned long nowMs, TelemetryPublisher& publisher) override;
  void appendSnapshot(JsonObject metrics, JsonObject sensorStatus) override;

 private:
  String readUid() const;

  MFRC522 reader_;
  String lastUid_;
  unsigned long lastReadMs_ = 0;
};

