#include "RotaryEncoderSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

RotaryEncoderSensor::RotaryEncoderSensor()
    : SensorModule("rotary_encoder"),
      encoder_(Config::kEncoderPinA, Config::kEncoderPinB) {}

void RotaryEncoderSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  pinMode(Config::kEncoderButtonPin, INPUT_PULLUP);
}

void RotaryEncoderSensor::emitMovementEvent(long delta, unsigned long nowMs, TelemetryPublisher& publisher) {
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "encoder_moved", nowMs, fields);
  fields["encoder_position"] = position_;
  fields["encoder_delta"] = delta;
  publisher.send(eventDoc);
}

void RotaryEncoderSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kEncoderIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;

  long newPosition = encoder_.read();
  delta_ = newPosition - position_;
  position_ = newPosition;
  if (delta_ != 0) {
    emitMovementEvent(delta_, nowMs, publisher);
  }

  buttonPressed_ = digitalRead(Config::kEncoderButtonPin) == LOW;
  if (buttonPressed_ == lastPublishedButton_) {
    return;
  }

  lastPublishedButton_ = buttonPressed_;
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "encoder_button_changed", nowMs, fields);
  fields["encoder_button"] = buttonPressed_;
  publisher.send(eventDoc);
}

void RotaryEncoderSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  metrics["encoder_position"] = position_;
  metrics["encoder_delta"] = delta_;
  metrics["encoder_button"] = buttonPressed_;

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

