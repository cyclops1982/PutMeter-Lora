#pragma once
#include <Arduino.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Wire.h>
#include <list>
#include "serialhelper.h"
#include "vl53l4cx_class.h"
using namespace Adafruit_LittleFS_Namespace;

#define SDA WB_I2C1_SDA
#define SCL WB_I2C1_SCL
#define XSHUT PIN_SERIAL2_RX


class SensorHelper
{
private:
  static const char caliberationfilename[];
  static void PrintCalData();
  static bool LoadCalData();
  static bool StoreCalData();
  static const char *ErrorToString(VL53L4CX_Error error);
  static bool Init();
  static bool initialized;


public:
  struct MeasuredObject
  {
    ushort MeasurementNr;
    ushort ObjectNr;
    uint DistanceInMM;
    const char *StatusText;
    short StatusCode;
    uint32_t Signal;
    uint32_t Ambient;
  };
  struct MeasurementResult
  {
    ushort NrOfMeasurements;
    ushort NrOfMeasurementsUsed;
    ushort PercentageFilled;
    uint DistanceInMM;
    const char *StatusText;
    short StatusCode;
    bool SuccesfulMeasurement;
    std::list<MeasuredObject> MeasuredObjects;
  };

  static bool RemoveCalib();
  static bool PerformCaliberation1();
  static bool PerformCaliberation2(int distanceInMM);
  static bool PerformCaliberation3();
  static MeasurementResult PerformMeasurement(uint offsetInMM, uint maxdepthInMM, uint ignoreMeasurementBelowInMM);
};
