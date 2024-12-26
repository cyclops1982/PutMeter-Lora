#pragma once
#include "serialhelper.h"

#define SEN_VL53L0X 1
#define SEN_RAK12500 2

#define SENSOR_TYPE SEN_RAK12500
class SensorHelper {
 public:
  static int GetDepthInMiliMeters();
};