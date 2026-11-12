#include "RfidSensor.h"

#include "../core/AppConfig.h"
#include "../core/TelemetryPublisher.h"

RfidSensor::RfidSensor() : SensorModule("rc522_rfid"), reader_(Config::kRfidSsPin, Config::kRfidResetPin) {}

void RfidSensor::begin(TelemetryPublisher& publisher) {
  (void)publisher;
  SPI.begin();
  reader_.PCD_Init();
}

String RfidSensor::readUid() const {
  String uid;
  for (byte index = 0; index < reader_.uid.size; ++index) {
    if (reader_.uid.uidByte[index] < 0x10) {
      uid += "0";
    }
    uid += String(reader_.uid.uidByte[index], HEX);
  }
  uid.toUpperCase();
  return uid;
}

void RfidSensor::tick(unsigned long nowMs, TelemetryPublisher& publisher) {
  if (nowMs - lastReadMs_ < Config::kRfidPollIntervalMs) {
    return;
  }

  lastReadMs_ = nowMs;
  if (!reader_.PICC_IsNewCardPresent() || !reader_.PICC_ReadCardSerial()) {
    return;
  }

  lastUid_ = readUid();

  DynamicJsonDocument eventDoc(Config::kEventDocumentCapacity);
  JsonObject fields;
  publisher.beginEvent(eventDoc, id(), "tag_scanned", nowMs, fields);
  fields["rfid_last_uid"] = lastUid_;
  publisher.send(eventDoc);

  reader_.PICC_HaltA();
  reader_.PCD_StopCrypto1();
}

void RfidSensor::appendSnapshot(JsonObject metrics, JsonObject sensorStatus) {
  if (lastUid_.length() > 0) {
    metrics["rfid_last_uid"] = lastUid_;
  }

  JsonObject status = sensorStatus.createNestedObject(id());
  status["connected"] = true;
  status["healthy"] = true;
  status["last_update_ms"] = lastReadMs_;
}

