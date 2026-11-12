#include "TelemetryPublisher.h"

#include "AppConfig.h"
#include "SensorModule.h"

TelemetryPublisher::TelemetryPublisher(Stream& transport) : transport_(transport) {}

void TelemetryPublisher::fillEnvelope(
    DynamicJsonDocument& document,
    const char* messageType,
    unsigned long nowMs) {
  document["schema_version"] = Config::kSchemaVersion;
  document["message_type"] = messageType;
  document["timestamp_ms"] = nowMs;
  document["sequence"] = sequence_++;
  document["device_id"] = Config::kDeviceId;
  document["transport"] = Config::kActiveTransport;
}

void TelemetryPublisher::publishSnapshot(
    unsigned long nowMs,
    SensorModule* const* modules,
    size_t moduleCount) {
  DynamicJsonDocument document(Config::kSnapshotDocumentCapacity);
  fillEnvelope(document, "telemetry", nowMs);

  JsonObject metrics = document.createNestedObject("metrics");
  JsonObject status = document.createNestedObject("sensor_status");

  for (size_t index = 0; index < moduleCount; ++index) {
    modules[index]->appendSnapshot(metrics, status);
  }

  send(document);
}

void TelemetryPublisher::beginEvent(
    DynamicJsonDocument& document,
    const char* source,
    const char* name,
    unsigned long nowMs,
    JsonObject& fields) {
  fillEnvelope(document, "event", nowMs);
  JsonObject event = document.createNestedObject("event");
  event["source"] = source;
  event["name"] = name;
  fields = event.createNestedObject("fields");
}

void TelemetryPublisher::send(const DynamicJsonDocument& document) {
  serializeJson(document, transport_);
  transport_.print('\n');
}

