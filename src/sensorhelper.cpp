#include "sensorhelper.h"

VL53L4CX sensor_vl53l4cx_sat(&Wire, XSHUT);
bool SensorHelper::initialized = false;

const char SensorHelper::caliberationfilename[] = "VL53L4CX_CALIBERATION_DATA.BIN";

const char *SensorHelper::ErrorToString(VL53L4CX_Error error)
{
  switch (error)
  {
  case VL53L4CX_ERROR_NONE:
    return "VL53L4CX_ERROR_NONE";
    break;
  case VL53L4CX_ERROR_CALIBRATION_WARNING:
    return "VL53L4CX_ERROR_CALIBRATION_WARNING";
    break;
  case VL53L4CX_ERROR_MIN_CLIPPED:
    return "VL53L4CX_ERROR_MIN_CLIPPED";
    break;
  case VL53L4CX_ERROR_UNDEFINED:
    return "VL53L4CX_ERROR_UNDEFINED";
    break;
  case VL53L4CX_ERROR_INVALID_PARAMS:
    return "VL53L4CX_ERROR_INVALID_PARAMS";
    break;
  case VL53L4CX_ERROR_NOT_SUPPORTED:
    return "VL53L4CX_ERROR_NOT_SUPPORTED";
    break;
  case VL53L4CX_ERROR_RANGE_ERROR:
    return "VL53L4CX_ERROR_RANGE_ERROR";
    break;
  case VL53L4CX_ERROR_TIME_OUT:
    return "VL53L4CX_ERROR_TIME_OUT";
    break;
  case VL53L4CX_ERROR_MODE_NOT_SUPPORTED:
    return "VL53L4CX_ERROR_MODE_NOT_SUPPORTED";
    break;
  case VL53L4CX_ERROR_BUFFER_TOO_SMALL:
    return "VL53L4CX_ERROR_BUFFER_TOO_SMALL";
    break;
  case VL53L4CX_ERROR_COMMS_BUFFER_TOO_SMALL:
    return "VL53L4CX_ERROR_COMMS_BUFFER_TOO_SMALL";
    break;
  case VL53L4CX_ERROR_GPIO_NOT_EXISTING:
    return "VL53L4CX_ERROR_GPIO_NOT_EXISTING";
    break;
  case VL53L4CX_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED:
    return "VL53L4CX_ERROR_GPIO_FUNCTIONALITY_NOT_SUPPORTED";
    break;
  case VL53L4CX_ERROR_CONTROL_INTERFACE:
    return "VL53L4CX_ERROR_CONTROL_INTERFACE";
    break;
  case VL53L4CX_ERROR_INVALID_COMMAND:
    return "VL53L4CX_ERROR_INVALID_COMMAND";
    break;
  case VL53L4CX_ERROR_DIVISION_BY_ZERO:
    return "VL53L4CX_ERROR_DIVISION_BY_ZERO";
    break;
  case VL53L4CX_ERROR_REF_SPAD_INIT:
    return "VL53L4CX_ERROR_REF_SPAD_INIT";
    break;
  case VL53L4CX_ERROR_GPH_SYNC_CHECK_FAIL:
    return "VL53L4CX_ERROR_GPH_SYNC_CHECK_FAIL";
    break;
  case VL53L4CX_ERROR_STREAM_COUNT_CHECK_FAIL:
    return "VL53L4CX_ERROR_STREAM_COUNT_CHECK_FAIL";
    break;
  case VL53L4CX_ERROR_GPH_ID_CHECK_FAIL:
    return "VL53L4CX_ERROR_GPH_ID_CHECK_FAIL";
    break;
  case VL53L4CX_ERROR_ZONE_STREAM_COUNT_CHECK_FAIL:
    return "VL53L4CX_ERROR_ZONE_STREAM_COUNT_CHECK_FAIL";
    break;
  case VL53L4CX_ERROR_ZONE_GPH_ID_CHECK_FAIL:
    return "VL53L4CX_ERROR_ZONE_GPH_ID_CHECK_FAIL";
    break;
  case VL53L4CX_ERROR_XTALK_EXTRACTION_NO_SAMPLE_FAIL:
    return "VL53L4CX_ERROR_XTALK_EXTRACTION_NO_SAMPLE_FAIL";
    break;
  case VL53L4CX_ERROR_XTALK_EXTRACTION_SIGMA_LIMIT_FAIL:
    return "VL53L4CX_ERROR_XTALK_EXTRACTION_SIGMA_LIMIT_FAIL";
    break;
  case VL53L4CX_ERROR_OFFSET_CAL_NO_SAMPLE_FAIL:
    return "VL53L4CX_ERROR_OFFSET_CAL_NO_SAMPLE_FAIL";
    break;
  case VL53L4CX_ERROR_OFFSET_CAL_NO_SPADS_ENABLED_FAIL:
    return "VL53L4CX_ERROR_OFFSET_CAL_NO_SPADS_ENABLED_FAIL";
    break;
  case VL53L4CX_ERROR_ZONE_CAL_NO_SAMPLE_FAIL:
    return "VL53L4CX_ERROR_ZONE_CAL_NO_SAMPLE_FAIL";
    break;
  case VL53L4CX_ERROR_TUNING_PARM_KEY_MISMATCH:
    return "VL53L4CX_ERROR_TUNING_PARM_KEY_MISMATCH";
    break;
  case VL53L4CX_WARNING_REF_SPAD_CHAR_NOT_ENOUGH_SPADS:
    return "VL53L4CX_WARNING_REF_SPAD_CHAR_NOT_ENOUGH_SPADS";
    break;
  case VL53L4CX_WARNING_REF_SPAD_CHAR_RATE_TOO_HIGH:
    return "VL53L4CX_WARNING_REF_SPAD_CHAR_RATE_TOO_HIGH";
    break;
  case VL53L4CX_WARNING_REF_SPAD_CHAR_RATE_TOO_LOW:
    return "VL53L4CX_WARNING_REF_SPAD_CHAR_RATE_TOO_LOW";
    break;
  case VL53L4CX_WARNING_OFFSET_CAL_MISSING_SAMPLES:
    return "VL53L4CX_WARNING_OFFSET_CAL_MISSING_SAMPLES";
    break;
  case VL53L4CX_WARNING_OFFSET_CAL_SIGMA_TOO_HIGH:
    return "VL53L4CX_WARNING_OFFSET_CAL_SIGMA_TOO_HIGH";
    break;
  case VL53L4CX_WARNING_OFFSET_CAL_RATE_TOO_HIGH:
    return "VL53L4CX_WARNING_OFFSET_CAL_RATE_TOO_HIGH";
    break;
  case VL53L4CX_WARNING_OFFSET_CAL_SPAD_COUNT_TOO_LOW:
    return "VL53L4CX_WARNING_OFFSET_CAL_SPAD_COUNT_TOO_LOW";
    break;
  case VL53L4CX_WARNING_ZONE_CAL_MISSING_SAMPLES:
    return "VL53L4CX_WARNING_ZONE_CAL_MISSING_SAMPLES";
    break;
  case VL53L4CX_WARNING_ZONE_CAL_SIGMA_TOO_HIGH:
    return "VL53L4CX_WARNING_ZONE_CAL_SIGMA_TOO_HIGH";
    break;
  case VL53L4CX_WARNING_ZONE_CAL_RATE_TOO_HIGH:
    return "VL53L4CX_WARNING_ZONE_CAL_RATE_TOO_HIGH";
    break;
  case VL53L4CX_WARNING_XTALK_MISSING_SAMPLES:
    return "VL53L4CX_WARNING_XTALK_MISSING_SAMPLES";
    break;
  case VL53L4CX_WARNING_XTALK_NO_SAMPLES_FOR_GRADIENT:
    return "VL53L4CX_WARNING_XTALK_NO_SAMPLES_FOR_GRADIENT";
    break;
  case VL53L4CX_WARNING_XTALK_SIGMA_LIMIT_FOR_GRADIENT:
    return "VL53L4CX_WARNING_XTALK_SIGMA_LIMIT_FOR_GRADIENT";
    break;
  case VL53L4CX_ERROR_NOT_IMPLEMENTED:
    return "VL53L4CX_ERROR_NOT_IMPLEMENTED";
    break;
  case VL53L4CX_ERROR_PLATFORM_SPECIFIC_START:
    return "VL53L4CX_ERROR_PLATFORM_SPECIFIC_START";
    break;
  default:
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "UNKNOWN: %u", error);
    return buffer;
  }
}

SensorHelper::MeasurementResult SensorHelper::PerformMeasurement(uint offsetInMM, uint maxdepthInMM, uint ignoreMeasurementBelowInMM)
{
  MeasurementResult result;
  result.SuccesfulMeasurement = false;

  // TODO: fill measurmentReuslt with redafult values so the return on failure is correct

  if (!Init())
  {
    result.StatusText = "Failed initialization";
    result.StatusCode = -1;
    return result;
  }

  VL53L4CX_Error status;
  if (InternalFS.exists(caliberationfilename))
  {
    File file(InternalFS);
    file.open(caliberationfilename, FILE_O_READ);
    //TODO: error handling 
    VL53L4CX_CalibrationData_t calData;
    file.readBytes((char *)&calData, sizeof(calData));
    file.close();
    SERIAL_LOG("Read %d bytes...", sizeof(calData));
    status = sensor_vl53l4cx_sat.VL53L4CX_SetCalibrationData(&calData);
    SERIAL_LOG("VL53L4CX_SetCalibrationData status: %s", SensorHelper::ErrorToString(status));
    if (status != VL53L4CX_ERROR_NONE)
    {
      result.StatusText = ErrorToString(status);
      result.StatusCode = -2;
      return result;
    }
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_SetXTalkCompensationEnable(1);
  SERIAL_LOG("VL53L4CX_SetXTalkCompensationEnable status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -3;
    return result;
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_SetMeasurementTimingBudgetMicroSeconds(33333);
  SERIAL_LOG("VL53L4CX_SetMeasurementTimingBudgetMicroSeconds status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -4;
    return result;
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_SetDistanceMode(VL53L4CX_DISTANCEMODE_LONG);
  SERIAL_LOG("VL53L4CX_SetDistanceMode status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -5;
    return result;
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_SetOffsetCorrectionMode(VL53L4CX_OFFSETCORRECTIONMODE_PERVCSEL);
  SERIAL_LOG("VL53L4CX_SetOffsetCorrectionMode status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -6;
    return result;
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_SmudgeCorrectionEnable(VL53L4CX_SMUDGE_CORRECTION_CONTINUOUS);
  SERIAL_LOG("VL53L4CX_SmudgeCorrectionEnable status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -7;
    return result;
  }

  status = sensor_vl53l4cx_sat.VL53L4CX_StartMeasurement();
  SERIAL_LOG("Started measurement status: %s", SensorHelper::ErrorToString(status));
  if (status != VL53L4CX_ERROR_NONE)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -8;
    return result;
  }

  VL53L4CX_MultiRangingData_t MultiRangingData;
  VL53L4CX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;

  result.NrOfMeasurements = 0;
  result.NrOfMeasurementsUsed = 0;
  result.DistanceInMM = 0;
  int noObjectCount = 0;
  int distance = 0;
  for (; result.NrOfMeasurements < 5; result.NrOfMeasurements++)
  {
    do
    {
      status = sensor_vl53l4cx_sat.VL53L4CX_GetMeasurementDataReady(&NewDataReady);
      if (status != VL53L4CX_ERROR_NONE)
      {
        result.StatusText = ErrorToString(status);
        result.StatusCode = -9;
        return result;
      }
      SERIAL_LOG("%d: Status of data: %d; NewData: %u", result.NrOfMeasurements, status, NewDataReady);
      delay(100);
    } while (!NewDataReady);

    SERIAL_LOG("%d: Status of data: %d; NewData: %u", result.NrOfMeasurements, status, NewDataReady);

    status = sensor_vl53l4cx_sat.VL53L4CX_GetMultiRangingData(pMultiRangingData);
    if (status != VL53L4CX_ERROR_NONE)
    {
      result.StatusText = ErrorToString(status);
      result.StatusCode = -10;
      return result;
    }
    SERIAL_LOG("%d: Status read: %d/%s", result.NrOfMeasurements, status, SensorHelper::ErrorToString(status));
    SERIAL_LOG("%d: VL53L4CX Satellite: Count=%d, #Objs=%1d ", result.NrOfMeasurements, pMultiRangingData->StreamCount,
               pMultiRangingData->NumberOfObjectsFound);
    if (pMultiRangingData->NumberOfObjectsFound == 0)
    {
      noObjectCount++;
    }
    for (int j = 0; j < pMultiRangingData->NumberOfObjectsFound; j++)
    {
      SERIAL_LOG("  %d: status=%s; Distance: %dmm", j, SensorHelper::ErrorToString(pMultiRangingData->RangeData[j].RangeStatus),
                 pMultiRangingData->RangeData[j].RangeMilliMeter);
      SERIAL_LOG("  %d: Signal=%f Mcps;Ambient=%f Mcps ", j, (float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0,
                 (float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
      // Check if the range is valid and above 10mm
      if (pMultiRangingData->RangeData[j].RangeStatus == VL53L4CX_RANGESTATUS_RANGE_VALID && pMultiRangingData->RangeData[j].RangeMilliMeter >= ignoreMeasurementBelowInMM)
      {
        result.NrOfMeasurementsUsed++;
        distance += pMultiRangingData->RangeData[j].RangeMilliMeter;
      }

      MeasuredObject obj;
      obj.ObjectNr = j;
      obj.MeasurementNr = result.NrOfMeasurements;
      obj.DistanceInMM = pMultiRangingData->RangeData[j].RangeMilliMeter;
      obj.StatusText = SensorHelper::ErrorToString(pMultiRangingData->RangeData[j].RangeStatus);
      obj.StatusCode = pMultiRangingData->RangeData[j].RangeStatus;
      obj.Signal = pMultiRangingData->RangeData[j].SignalRateRtnMegaCps;
      obj.Ambient = pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps;
      result.MeasuredObjects.push_back(obj);
    }
    status = sensor_vl53l4cx_sat.VL53L4CX_ClearInterruptAndStartMeasurement();
    if (status != VL53L4CX_ERROR_NONE)
    {
      result.StatusText = ErrorToString(status);
      result.StatusCode = -11;
      return result;
    }

    delay(200);
  } // for i<5;

  // This means we did measurements and in all of them we didn't find any objects
  if (result.NrOfMeasurements == noObjectCount)
  {
    result.StatusText = ErrorToString(status);
    result.StatusCode = -12;
    return result;
  }

  if (result.NrOfMeasurementsUsed == 0)
  { // We found objects (see previous if statement), but we didn't get any RANGESTATUS_RANGE_VALID
    result.StatusText = ErrorToString(status);
    result.StatusCode = -13;
    return result;
  }
  result.DistanceInMM = (distance / result.NrOfMeasurementsUsed);

  if (result.DistanceInMM >= maxdepthInMM)
  {
    result.PercentageFilled = 0;
  }
  else if (result.DistanceInMM <= offsetInMM)
  {
    result.PercentageFilled = 100;
  }
  else
  {
    auto totalheight = maxdepthInMM - offsetInMM;
    uint remaining = maxdepthInMM - result.DistanceInMM;
    result.PercentageFilled = (remaining / totalheight) * 100;
  }

  SERIAL_LOG("RESULT = %d", result.DistanceInMM);
  return result;
}

bool SensorHelper::Init()
{
  if (!initialized)
  {
    pinMode(SDA, INPUT);
    pinMode(SCL, OUTPUT);
    delay(200);

    Wire.setPins(SDA, SCL);
    Wire.begin();
    

    delay(100);
    int res = sensor_vl53l4cx_sat.begin();
    SERIAL_LOG("sensor.begin(): %d", res);
    if (res != 0)
    {
      return false;
    }
    delay(100);
    VL53L4CX_Error status = sensor_vl53l4cx_sat.InitSensor(0x12);
    SERIAL_LOG("InitSensor status: %s", SensorHelper::ErrorToString(status));
    if (status != VL53L4CX_ERROR_NONE)
    {
      return false;
    }

    InternalFS.begin();

    initialized = true;
  }
  return initialized;
}

bool SensorHelper::StoreCalData()
{
  VL53L4CX_CalibrationData_t calDataDummy;
  VL53L4CX_CalibrationData_t *calData = &calDataDummy;
  VL53L4CX_Error status = sensor_vl53l4cx_sat.VL53L4CX_GetCalibrationData(calData);
  SERIAL_LOG("StoreCalData()  - VL53L4CX_GetCalibrationData status: %s", SensorHelper::ErrorToString(status));

  if (InternalFS.exists(caliberationfilename))
  {
    SERIAL_LOG("StoreCalData() - Existing caliberation file exists, removing!");
    InternalFS.remove(caliberationfilename);
  }
  File file = InternalFS.open(caliberationfilename, FILE_O_WRITE);
  if (!file)
  {
    SERIAL_LOG("StoreCalData() - FAILED TO OPEN FILE!: %d", file);
    return false;
  }

  auto size = file.write((uint8_t *)calData, sizeof(*calData));
  file.close();
  SERIAL_LOG("StoreCalData() - Writen %d data. exiting", size);
  return true;
}

bool SensorHelper::LoadCalData()
{
  File file = InternalFS.open(caliberationfilename, FILE_O_READ);
  if (file)
  {
    VL53L4CX_CalibrationData_t calData;
    file.readBytes((char *)&calData, sizeof(calData));
    SERIAL_LOG("SetCalData() - Read %d bytes...", sizeof(calData));
    VL53L4CX_Error status = sensor_vl53l4cx_sat.VL53L4CX_SetCalibrationData(&calData);
    SERIAL_LOG("SetCalData() - VL53L4CX_SetCalibrationData status: %s", SensorHelper::ErrorToString(status));
    file.close();
    return true;
  }
  SERIAL_LOG("SetCalData() - failed, couldn't get file.");
  return false;
}

void SensorHelper::PrintCalData()
{
  VL53L4CX_CalibrationData_t calDataDummy;
  VL53L4CX_CalibrationData_t *data = &calDataDummy;
  VL53L4CX_Error status = sensor_vl53l4cx_sat.VL53L4CX_GetCalibrationData(data);
  SERIAL_LOG("PrintCalData() - VL53L4CX_GetCalibrationData status: %s", SensorHelper::ErrorToString(status));

  SERIAL_LOG("==== CALIBERATION DATA ==== ");
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_0: %u", data->customer.global_config__spad_enables_ref_0);
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_1: %u", data->customer.global_config__spad_enables_ref_1);
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_2: %u", data->customer.global_config__spad_enables_ref_2);
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_3: %u", data->customer.global_config__spad_enables_ref_3);
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_4: %u", data->customer.global_config__spad_enables_ref_4);
  SERIAL_LOG("data->customer.global_config__spad_enables_ref_5: %u", data->customer.global_config__spad_enables_ref_5);
  SERIAL_LOG("data->customer.global_config__ref_en_start_select: %u", data->customer.global_config__ref_en_start_select);
  SERIAL_LOG("data->customer.ref_spad_man__num_requested_ref_spads: %u", data->customer.ref_spad_man__num_requested_ref_spads);
  SERIAL_LOG("data->customer.ref_spad_man__ref_location: %u", data->customer.ref_spad_man__ref_location);
  SERIAL_LOG("data->customer.algo__crosstalk_compensation_plane_offset_kcps: %u", data->customer.algo__crosstalk_compensation_plane_offset_kcps);
  SERIAL_LOG("data->customer.algo__crosstalk_compensation_x_plane_gradient_kcps: %d",
             data->customer.algo__crosstalk_compensation_x_plane_gradient_kcps);
  SERIAL_LOG("data->customer.algo__crosstalk_compensation_y_plane_gradient_kcps: %d",
             data->customer.algo__crosstalk_compensation_y_plane_gradient_kcps);
  SERIAL_LOG("data->customer.ref_spad_char__total_rate_target_mcps: %u", data->customer.ref_spad_char__total_rate_target_mcps);
  SERIAL_LOG("data->customer.algo__part_to_part_range_offset_mm: %d", data->customer.algo__part_to_part_range_offset_mm);
  SERIAL_LOG("data->customer.mm_config__inner_offset_mm: %d", data->customer.mm_config__inner_offset_mm);
  SERIAL_LOG("data->customer.mm_config__outer_offset_mm: %d", data->customer.mm_config__outer_offset_mm);
}

bool SensorHelper::RemoveCalib()
{
  if (InternalFS.exists(caliberationfilename))
  {
    SERIAL_LOG("Existing caliberation file exists, removing!");
    InternalFS.remove(caliberationfilename);
    return true;
  }
  return false;
}

bool SensorHelper::PerformCaliberation1()
{
  SERIAL_LOG("=== Caliberation 1 - VL53L4CX_PerformRefSpadManagement ===");

  VL53L4CX_Error status;
  status = sensor_vl53l4cx_sat.VL53L4CX_PerformRefSpadManagement();
  SERIAL_LOG("VL53L4CX_PerformRefSpadManagement status: %s", SensorHelper::ErrorToString(status));

  SensorHelper::PrintCalData();

  return SensorHelper::StoreCalData();
}

bool SensorHelper::PerformCaliberation2(int distanceInMM)
{
  SERIAL_LOG("=== Caliberation 2 - VL53L4CX_PerformOffsetPerVcselCalibration(%d) ===", distanceInMM);

  if (!SensorHelper::LoadCalData())
  {
    SERIAL_LOG("PerformCaliberation2() - Failed to SetCalData - make sure you run calibration 1 first.");
    return false;
  }

  VL53L4CX_Error status = sensor_vl53l4cx_sat.VL53L4CX_PerformOffsetPerVcselCalibration(distanceInMM);
  SERIAL_LOG("PerformCaliberation2() - VL53L4CX_PerformOffsetPerVcselCalibration status: %s", SensorHelper::ErrorToString(status));

  SensorHelper::PrintCalData();

  if (!SensorHelper::StoreCalData())
  {
    SERIAL_LOG("PerformCaliberation2() - Failed to store calibration data");
    return false;
  }

  return true;
}

bool SensorHelper::PerformCaliberation3()
{
  SERIAL_LOG("=== Caliberation 3 - VL53L4CX_PerformXTalkCalibration ===");
  if (!SensorHelper::LoadCalData())
  {
    SERIAL_LOG("PerformCaliberation3() - Failed to SetCalData - make sure you run calibration 1 first.");
    return false;
  }

  VL53L4CX_Error status = sensor_vl53l4cx_sat.VL53L4CX_PerformXTalkCalibration();
  SERIAL_LOG("PerformCaliberation3() - VL53L4CX_PerformXTalkCalibration status: %s", SensorHelper::ErrorToString(status));

  SensorHelper::PrintCalData();

  if (!SensorHelper::StoreCalData())
  {
    SERIAL_LOG("PerformCaliberation3() - Failed to store calibration data");
    return false;
  }

  return true;
}
