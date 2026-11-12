#pragma once

#include "AppConfig.h"
#include "SensorModule.h"
#include "TelemetryPublisher.h"

#if ENABLE_SENSOR_DHT11
#include "../sensors/DhtSensor.h"
#endif
#if ENABLE_SENSOR_PIR
#include "../sensors/PirSensor.h"
#endif
#if ENABLE_SENSOR_ULTRASONIC
#include "../sensors/UltrasonicSensor.h"
#endif
#if ENABLE_SENSOR_SOUND
#include "../sensors/SoundSensor.h"
#endif
#if ENABLE_SENSOR_RAIN
#include "../sensors/RainSensor.h"
#endif
#if ENABLE_SENSOR_JOYSTICK
#include "../sensors/JoystickSensor.h"
#endif
#if ENABLE_SENSOR_TOUCH
#include "../sensors/TouchSensor.h"
#endif
#if ENABLE_SENSOR_ROTARY_ENCODER
#include "../sensors/RotaryEncoderSensor.h"
#endif
#if ENABLE_SENSOR_PUSHBUTTON
#include "../sensors/PushButtonSensor.h"
#endif
#if ENABLE_SENSOR_IR_RECEIVER
#include "../sensors/IrReceiverSensor.h"
#endif
#if ENABLE_SENSOR_RFID
#include "../sensors/RfidSensor.h"
#endif
#if ENABLE_SENSOR_KEYPAD
#include "../sensors/KeypadSensor.h"
#endif

class FirmwareApp {
 public:
  FirmwareApp();
  void begin();
  void loop();

 private:
  void registerSensor(SensorModule& module);

  TelemetryPublisher publisher_;
  SensorModule* modules_[Config::kMaxSensors];
  size_t moduleCount_ = 0;
  unsigned long lastSnapshotMs_ = 0;

#if ENABLE_SENSOR_DHT11
  DhtSensor dht_;
#endif
#if ENABLE_SENSOR_PIR
  PirSensor pir_;
#endif
#if ENABLE_SENSOR_ULTRASONIC
  UltrasonicSensor ultrasonic_;
#endif
#if ENABLE_SENSOR_SOUND
  SoundSensor sound_;
#endif
#if ENABLE_SENSOR_RAIN
  RainSensor rain_;
#endif
#if ENABLE_SENSOR_JOYSTICK
  JoystickSensor joystick_;
#endif
#if ENABLE_SENSOR_TOUCH
  TouchSensor touch_;
#endif
#if ENABLE_SENSOR_ROTARY_ENCODER
  RotaryEncoderSensor encoder_;
#endif
#if ENABLE_SENSOR_PUSHBUTTON
  PushButtonSensor pushButton_;
#endif
#if ENABLE_SENSOR_IR_RECEIVER
  IrReceiverSensor irReceiver_;
#endif
#if ENABLE_SENSOR_RFID
  RfidSensor rfid_;
#endif
#if ENABLE_SENSOR_KEYPAD
  KeypadSensor keypad_;
#endif
};

