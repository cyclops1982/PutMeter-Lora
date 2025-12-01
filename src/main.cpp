#include <stdio.h>
#include <Wire.h>
#include "batteryhelper.h"
#include "lorahelper.h"
#include "ledhelper.h"
#include "serialhelper.h"
#include "sensorhelper.h"
#include "main.h"
#include "config.h"
#include "version.h"


ConfigHelper g_configParams;
SoftwareTimer g_taskWakeupTimer;
SoftwareTimer g_taskClear1stMotionInterruptTimer;
SoftwareTimer g_taskClear2ndMotionInterruptTimer;
uint16_t g_msgcount = 0;

SemaphoreHandle_t g_semaphore = NULL;
BaseType_t g_taskHighPrio = pdTRUE;
EventType g_EventType = EventType::None;

uint8_t g_rcvdLoRaData[LORAWAN_BUFFER_SIZE];
uint8_t g_rcvdDataLen = 0;
bool g_lorawan_joined = false;

bool g_do1stMotionUpdate = true;
bool g_do2ndMotionUpdate = true;



void periodicWakeup(TimerHandle_t unused)
{
  g_EventType |= EventType::Timer;
  xSemaphoreGiveFromISR(g_semaphore, &g_taskHighPrio);
}

void setup()
{
  delay(1000); // For whatever reason, some pins/things are not available at startup right away. So we wait for a bit. This also helps when we want to connect to console.
  LedHelper::init();
  // Initialize serial for output.
#ifndef MAX_SAVE
  time_t timeout = millis();
  Serial.begin(115200);
  // check if serial has become available and if not, just wait for it.
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
#endif
  SERIAL_LOG("Setup start.");
  SERIAL_LOG("Starting %s", VERSIONSTRING)
  delay(500);
  g_semaphore = xSemaphoreCreateBinary();

  LedHelper::init();

  if (!g_configParams.InitConfig())
  {
    LedHelper::BlinkHalt(2);
  }
  delay(1000);

#ifndef LORAWAN_FAKE
  // Lora stuff
  LoraHelper::InitAndJoin(g_configParams.GetLoraDataRate(), g_configParams.GetLoraTXPower(), g_configParams.GetLoraADREnabled(),
                          g_configParams.GetLoraDevEUI(), g_configParams.GetLoraNodeAppEUI(), g_configParams.GetLoraAppKey());
#endif

  // Go into sleep mode
  g_EventType = EventType::Timer;
  xSemaphoreGive(g_semaphore);
  g_taskWakeupTimer.begin(g_configParams.GetSleepTime0InSeconds() * 1000, periodicWakeup);
  g_taskWakeupTimer.start();
}

bool SendData()
{
#ifndef LORAWAN_FAKE
  if (!g_lorawan_joined)
  {
    SERIAL_LOG("Lora not joined yet while trying to send. Joining now.");
    LoraHelper::InitAndJoin(g_configParams.GetLoraDataRate(), g_configParams.GetLoraTXPower(), g_configParams.GetLoraADREnabled(),
                            g_configParams.GetLoraDevEUI(), g_configParams.GetLoraNodeAppEUI(), g_configParams.GetLoraAppKey());
  }
  if (g_lorawan_joined)
  {
    lmh_confirm needConfirm = (lmh_confirm)g_configParams.GetLoraRequireConfirmation();
    for (ushort attempt = 0; attempt < 5; attempt++)
    {
      lmh_error_status loraSendState = lmh_send(&g_SendLoraData, needConfirm);
      SERIAL_LOG("lmh_send result: %d; Confirmation needed?: %d", loraSendState, needConfirm);
      // lmh_send can return LMH_SUCCESS, LMH_BUSY or LMH_ERROR
      if (loraSendState == LMH_ERROR)
      {
        SERIAL_LOG("Failed to LMH_SEND due to LMH_ERROR");
        LedHelper::BlinkStatus(5);
      }
      if (loraSendState == LMH_BUSY)
      {
        LedHelper::BlinkStatus(3);

        int totalDelay = 1000 * std::pow(2, attempt);
        SERIAL_LOG("Exponential waiting for confirmation: %d", totalDelay);
        delay(totalDelay);
      }
      if (loraSendState == LMH_SUCCESS)
      {
        SERIAL_LOG("LMH_SEND succeeded");
        return true;
      }
    }

    return false;
  }
  SERIAL_LOG("SKIPPING SEND - We are not joined to a network");
  return false;
#else
  SERIAL_LOG("NOT SENDING lorawan packages as we have LORAWAN_FAKE set. Data that would be send:");
  for (uint8_t x = 0; x < g_SendLoraData.buffsize; x++)
  {
    SERIAL_LOG("%d: 0x%02X", x, g_SendLoraData.buffer[x]);
  }
  return true;
#endif
}

void handleReceivedMessage()
{
  if (g_rcvdDataLen == 0)
  {
    SERIAL_LOG("Running handleReceivedMessage, but g_rcvdDataLen is 0");
    return;
  }
  /*  for (uint8_t i = 0; i < g_rcvdDataLen; i++)
  {
    char hexstr[5];
    sprintf(hexstr, "0x%02X", g_rcvdLoRaData[i]);
    SERIAL_LOG("DATA %d: %s", i, hexstr)
  }
  */

  g_configParams.SetConfig(g_rcvdLoRaData, g_rcvdDataLen);

  // Some parameters require some re-initialization, which is what we do here for those cases.
  size_t arraySize = 0;
  ConfigOption *configs = g_configParams.GetConfigs(&arraySize);
  for (uint8_t i = 0; i < g_rcvdDataLen; i++)
  {
    for (size_t x = 0; x < arraySize; x++)
    {
      ConfigOption conf = configs[x];
      if (conf.configType == g_rcvdLoRaData[i])
      {
        SERIAL_LOG("Updating setting %s", conf.name);
        switch (g_rcvdLoRaData[i])
        {
        case ConfigType::SleepTime0:
          SERIAL_LOG("Timers are now %u ", g_configParams.GetSleepTime0InSeconds());
          // g_taskWakeupTimer.stop();
          g_taskWakeupTimer.setPeriod(g_configParams.GetSleepTime0InSeconds() * 1000);
          g_taskWakeupTimer.start();
          break;
        case ConfigType::LORA_ADREnabled:
        case ConfigType::LORA_DataRate:
          SERIAL_LOG("Setting Lora DataRate to %u and ARD to %d", g_configParams.GetLoraDataRate(), g_configParams.GetLoraADREnabled());
          LoraHelper::SetDataRate(g_configParams.GetLoraDataRate(), g_configParams.GetLoraADREnabled());
          break;
        case ConfigType::LORA_TXPower:
          SERIAL_LOG("Setting Lora TX Power to %d", g_configParams.GetLoraTXPower());
          LoraHelper::SetTXPower(g_configParams.GetLoraTXPower());
          break;
        case ConfigType::RemoveCalData:
          SERIAL_LOG("Removing caliberation data as per request.");
          SensorHelper::RemoveCalib();
          break;
        case ConfigType::Caliberate1:
          SERIAL_LOG("Starting caliberation step 1 as per request.");
          SensorHelper::PerformCaliberation1();
          break;
        case ConfigType::Caliberate2:
          SERIAL_LOG("Starting caliberation step 2 as per request.");
          SensorHelper::PerformCaliberation2(600); // 1 meter
          break;
        case ConfigType::Caliberate3:
          SERIAL_LOG("Starting caliberation step 3 as per request.");
          SensorHelper::PerformCaliberation3();
          break;
        }
        i += conf.sizeOfOption; // jump to the next one
        break;
      }
    }
  }
  g_rcvdDataLen = 0;
}

void doPeriodicUpdate()
{
  SERIAL_LOG("doPeriodicUpdate()");

  // powersave has a few modes. We use INVALID for managing the power via IO2.
  // We set it HIGH ALWAYS here because you might have come out of a other mode.
  // IO2 is the power to the sensors, so that basically turns it on/off when we need it.
  digitalWrite(WB_IO2, HIGH);

  uint16_t vbat_mv = BatteryHelper::readVBAT();
  SensorHelper::MeasurementResult measurementResult = SensorHelper::PerformMeasurement(0, 10000);
  int depthInMM = measurementResult.DistanceInMM;
  SERIAL_LOG("vbat: %u; DepthInMM: %d", vbat_mv, depthInMM);

  // Create the lora message
  memset(g_SendLoraData.buffer, 0, LORAWAN_BUFFER_SIZE);
  int size = 0;
  g_SendLoraData.port = 2;
  g_SendLoraData.buffer[size++] = 0x06;
  g_SendLoraData.buffer[size++] = 0x08;
  g_SendLoraData.buffer[size++] = vbat_mv >> 8;
  g_SendLoraData.buffer[size++] = vbat_mv;

  // Distance
  g_SendLoraData.buffer[size++] = depthInMM >> 8;
  g_SendLoraData.buffer[size++] = depthInMM;

  // Percentage
  if (depthInMM > 0)
  {
    uint16_t tankDepth = g_configParams.GetTankDepth();
    int depthInCM = depthInMM / 10;
    if (depthInCM <= tankDepth)
    {
      auto remaining = tankDepth - depthInCM;
      SERIAL_LOG("Depth of tank: %u", tankDepth);
      auto percentage = ((float)remaining / (float)tankDepth) * 100.0f;
      uint8_t percentageResult = static_cast<uint8_t>(percentage);
      g_SendLoraData.buffer[size++] = percentageResult;
      SERIAL_LOG("Calculated percentage: %u", percentageResult);
    }
    else
    {
      g_SendLoraData.buffer[size++] = 0xFE;
    }
  }
  else
  {
    // Percentage
    g_SendLoraData.buffer[size++] = 0xFF;
  }

  g_SendLoraData.buffer[size++] = g_msgcount >> 8;
  g_SendLoraData.buffer[size++] = g_msgcount;

  g_SendLoraData.buffsize = size;
  SendData();

  g_msgcount++;
};

void loop()
{
  SERIAL_LOG("loop() - waiting for semaphore");
  xSemaphoreTake(g_semaphore, portMAX_DELAY);
#ifndef MAX_SAVE
  digitalWrite(LED_GREEN, HIGH); // indicate we're doing stuff
#endif
  SERIAL_LOG("Semaphore taken with eventype: %d", g_EventType);

  if ((g_EventType & EventType::LoraDataReceived) == EventType::LoraDataReceived)
  {
    handleReceivedMessage();
    g_EventType &= ~EventType::LoraDataReceived;
  }
  if ((g_EventType & EventType::Timer) == EventType::Timer)
  {
    doPeriodicUpdate();
    g_EventType &= ~EventType::Timer;
  }

#ifndef MAX_SAVE
  digitalWrite(LED_GREEN, LOW);
#endif
}
