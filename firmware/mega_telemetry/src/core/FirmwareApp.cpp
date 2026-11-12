#include "FirmwareApp.h"

FirmwareApp::FirmwareApp() : publisher_(Serial) {
#if ENABLE_SENSOR_DHT11
  registerSensor(dht_);
#endif
#if ENABLE_SENSOR_PIR
  registerSensor(pir_);
#endif
#if ENABLE_SENSOR_ULTRASONIC
  registerSensor(ultrasonic_);
#endif
#if ENABLE_SENSOR_SOUND
  registerSensor(sound_);
#endif
#if ENABLE_SENSOR_RAIN
  registerSensor(rain_);
#endif
#if ENABLE_SENSOR_JOYSTICK
  registerSensor(joystick_);
#endif
#if ENABLE_SENSOR_TOUCH
  registerSensor(touch_);
#endif
#if ENABLE_SENSOR_ROTARY_ENCODER
  registerSensor(encoder_);
#endif
#if ENABLE_SENSOR_PUSHBUTTON
  registerSensor(pushButton_);
#endif
#if ENABLE_SENSOR_IR_RECEIVER
  registerSensor(irReceiver_);
#endif
#if ENABLE_SENSOR_RFID
  registerSensor(rfid_);
#endif
#if ENABLE_SENSOR_KEYPAD
  registerSensor(keypad_);
#endif
}

void FirmwareApp::registerSensor(SensorModule& module) {
  if (moduleCount_ < Config::kMaxSensors) {
    modules_[moduleCount_++] = &module;
  }
}

void FirmwareApp::begin() {
  Serial.begin(Config::kBaudRate);
  while (!Serial) {
    delay(10);
  }

  // New modules can be added by creating another SensorModule implementation,
  // including it in FirmwareApp.h, and registering it in the constructor.
  for (size_t index = 0; index < moduleCount_; ++index) {
    modules_[index]->begin(publisher_);
  }
}

void FirmwareApp::loop() {
  unsigned long nowMs = millis();

  for (size_t index = 0; index < moduleCount_; ++index) {
    modules_[index]->tick(nowMs, publisher_);
  }

  if (nowMs - lastSnapshotMs_ >= Config::kSnapshotIntervalMs) {
    publisher_.publishSnapshot(nowMs, modules_, moduleCount_);
    lastSnapshotMs_ = nowMs;
  }
}

