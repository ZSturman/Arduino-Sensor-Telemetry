#pragma once

#include <ArduinoJson.h>

class SensorModule;

class TelemetryPublisher {
 public:
  explicit TelemetryPublisher(Stream& transport);

  void publishSnapshot(unsigned long nowMs, SensorModule* const* modules, size_t moduleCount);
  void beginEvent(
      DynamicJsonDocument& document,
      const char* source,
      const char* name,
      unsigned long nowMs,
      JsonObject& fields);
  void send(const DynamicJsonDocument& document);

 private:
  void fillEnvelope(DynamicJsonDocument& document, const char* messageType, unsigned long nowMs);

  Stream& transport_;
  unsigned long sequence_ = 0;
};

