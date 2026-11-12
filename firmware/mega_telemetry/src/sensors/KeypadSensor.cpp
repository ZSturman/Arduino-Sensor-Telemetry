#include "KeypadSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

KeypadSensor::KeypadSensor() : SensorModule("keypad_4x4") {
  const char defaults[4][4] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'},
  };

  for (byte row = 0; row < 4; ++row) {
    rowPins_[row] = Config::kKeypadRowPins[row];
    columnPins_[row] = Config::kKeypadColumnPins[row];
    for (byte column = 0; column < 4; ++column) {
      keymap_[row][column] = defaults[row][column];
    }
  }
}

void KeypadSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  keypad_ = new Keypad(makeKeymap(keymap_), rowPins_, columnPins_, 4, 4);
}

void KeypadSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kKeypadIntervalMs || keypad_ == nullptr) {
    return;
  }

  lastReadMs_ = nowMs;
  char key = keypad_->getKey();
  if (key == NO_KEY) {
    return;
  }

  lastKey_ = String(key);
  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "key_pressed", nowMs, fields);
  fields["keypad_last_key"] = lastKey_;
  publisher.send(eventDoc);
}

void KeypadSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  if (lastKey_.length() > 0) {
    metrics["keypad_last_key"] = lastKey_;
  }

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = keypad_ != nullptr;
  status["last_update_ms"] = lastReadMs_;
}

