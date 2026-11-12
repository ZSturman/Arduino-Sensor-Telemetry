#include "IrReceiverSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

IrReceiverSensor::IrReceiverSensor() : SensorModule("ir_receiver") {}

void IrReceiverSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  IrReceiver.begin(Config::kIrReceiverPin, DISABLE_LED_FEEDBACK);
}

void IrReceiverSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kIrPollIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  if (!IrReceiver.decode()) {
    return;
  }

  String code = "0x";
  code += String(static_cast<unsigned long>(IrReceiver.decodedIRData.decodedRawData), HEX);
  code.toUpperCase();
  lastCodeHex_ = code;

  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "ir_received", nowMs, fields);
  fields["ir_last_code_hex"] = lastCodeHex_;
  fields["protocol"] = getProtocolString(IrReceiver.decodedIRData.protocol);
  publisher.send(eventDoc);

  IrReceiver.resume();
}

void IrReceiverSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  if (lastCodeHex_.length() > 0) {
    metrics["ir_last_code_hex"] = lastCodeHex_;
  }

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

