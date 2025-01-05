#include "sensorhelper.h"

#include <Arduino.h>

#if SENSOR_TYPE == SEN_VL53L0X
#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#endif

int SensorHelper::GetDepthInMiliMeters()
{
#if SENSOR_TYPE == SEN_RAK12500
#define RATIO 346.6 / 1000 / 2  // velocity of sound =331.6+0.6*25℃(m/s),(Indoor temperature about 25℃)

  // setup code
  pinMode(WB_I2C1_SDA, INPUT);
  pinMode(WB_I2C1_SCL, OUTPUT);
  digitalWrite(WB_I2C1_SCL, LOW);

  // measurement code
  digitalWrite(WB_I2C1_SCL, HIGH);
  delayMicroseconds(12);  // pull high time need over 10us
  digitalWrite(WB_I2C1_SCL, LOW);
  unsigned long respondTime = pulseIn(WB_I2C1_SDA, HIGH);  // microseconds
  SERIAL_LOG("Response time from RAK12500: %u", respondTime);

  if((respondTime > 0) && (respondTime < 24125))  // SDA pin max timeout is 33000us according it's datasheet
  {
    return respondTime * RATIO;
  }
#endif
#if SENSOR_TYPE == SEN_VL53L0X
  Adafruit_VL53L0X tofSensor = Adafruit_VL53L0X();
  digitalWrite(WB_IO2, HIGH);
  pinMode(WB_I2C1_SDA, INPUT);
  pinMode(WB_I2C1_SCL, OUTPUT);
  delay(200);
  if(!tofSensor.begin(VL53L0X_I2C_ADDR, true, &Wire, Adafruit_VL53L0X::VL53L0X_Sense_config_t::VL53L0X_SENSE_LONG_RANGE)) {
    SERIAL_LOG("FAILED TO INITIALIZE TOF SENSOR!");
  }
  else {
    VL53L0X_RangingMeasurementData_t measure;
    VL53L0X_Error err = tofSensor.getSingleRangingMeasurement(&measure, true);  // pass in 'true' to get debug data printout!
    SERIAL_LOG("READ RETURN: %d\n", err);
    SERIAL_LOG("RangeStatus: %d\n", measure.RangeStatus);
    if(measure.RangeStatus != 4) {  // phase failures have incorrect data
      return measure.RangeMilliMeter;
    }
    else {
      SERIAL_LOG("Failed to read");
    }
  }

#endif
  return -1;
}