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
uint16_t g_msgcount = 0;

SemaphoreHandle_t g_taskEvent = NULL;
EventType g_EventType = EventType::None;
uint8_t g_rcvdLoRaData[LORAWAN_BUFFER_SIZE];
uint8_t g_rcvdDataLen = 0;
bool g_lorawan_joined = false;
bool g_lorawan_msgconfirmed = false;

void periodicWakeup(TimerHandle_t unused)
{
  // Give the semaphore, so the loop task will wake up
  g_EventType = EventType::Timer;
  xSemaphoreGiveFromISR(g_taskEvent, pdFALSE);
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

  if (!g_configParams.InitConfig())
  {
    LedHelper::BlinkHalt(2);
  }
  delay(1000);
  // Create semaphore for task handling.
  g_taskEvent = xSemaphoreCreateBinary();

  // Turn on power to sensors
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(100);

#ifndef LORAWAN_FAKE
  // Lora stuff
  LoraHelper::InitAndJoin(g_configParams.GetLoraDataRate(), g_configParams.GetLoraTXPower(), g_configParams.GetLoraADREnabled(),
                          g_configParams.GetLoraDevEUI(), g_configParams.GetLoraNodeAppEUI(), g_configParams.GetLoraAppKey());
#endif

  // Go into sleep mode
  xSemaphoreGive(g_taskEvent);
  g_EventType = EventType::Timer;
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

    g_lorawan_msgconfirmed = false; // gets set in lorahelper.cpp
    lmh_confirm needConfirm = (lmh_confirm)g_configParams.GetLoraRequireConfirmation();
    for (ushort attempt = 0; attempt < 5; attempt++)
    {
      lmh_error_status loraSendState = lmh_send(&g_SendLoraData, needConfirm);
      SERIAL_LOG("lmh_send result: %d; Confirmation needed?: %d", loraSendState, needConfirm);
      if (loraSendState == LMH_SUCCESS) // this status just means that we could send. As in, it's not busy or an error. It does not mean that the CONFIRMATION worked.
      {
        if (needConfirm == LMH_CONFIRMED_MSG)
        {
          for (ushort confirmationCount = 0; confirmationCount < 5; confirmationCount++)
          {
            int totalDelay = 1000 * std::pow(2, confirmationCount);
            SERIAL_LOG("Exponential waiting for confirmation: %d", totalDelay);
            delay(totalDelay);
            SERIAL_LOG("msg_confirmred?: %d", g_lorawan_msgconfirmed);
            if (g_lorawan_msgconfirmed == true)
            {
              break;
            }
          }
          return g_lorawan_msgconfirmed;
        }
        return true;
      }
      else
      {
        int totalDelay = 2000 * std::pow(2, attempt);
        SERIAL_LOG("Exponential waiting for lora to send: %d", totalDelay)
        delay(totalDelay);
      }
    }
    LedHelper::BlinkStatus(3);
    return false;
  }
  else
  {
    SERIAL_LOG("SKIPPING SEND - We are not joined to a network");
  }
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
  int depthInMM = SensorHelper::GetDepthInMiliMeters();

  // Create the lora message
  memset(g_SendLoraData.buffer, 0, LORAWAN_BUFFER_SIZE);
  int size = 0;
  g_SendLoraData.port = 2;
  g_SendLoraData.buffer[size++] = 0x06;
  g_SendLoraData.buffer[size++] = 0x08;
  g_SendLoraData.buffer[size++] = vbat_mv >> 8;
  g_SendLoraData.buffer[size++] = vbat_mv;

  if (depthInMM > 0)
  {
    // Distance
    g_SendLoraData.buffer[size++] = depthInMM >> 8;
    g_SendLoraData.buffer[size++] = depthInMM;

    // Percentage
    uint16_t tankDepth = g_configParams.GetTankDepth();
    int depthInCM = depthInMM/10;
    if (depthInCM < tankDepth)
    {
      auto remaining = tankDepth - depthInCM;
      auto percentage = ((float)remaining / (float)tankDepth) * 100.0f;
      uint8_t percentageResult = static_cast<uint8_t>(percentage);
      g_SendLoraData.buffer[size++] = percentageResult;
      
    }
    else
    {
      g_SendLoraData.buffer[size++] = 0xFE;
    }
  }
  else
  {
    // Distance
    g_SendLoraData.buffer[size++] = 0;
    g_SendLoraData.buffer[size++] = 0;
    // Percentage
    g_SendLoraData.buffer[size++] = 0xFF;
  }

  g_SendLoraData.buffer[size++] = g_msgcount >> 8;
  g_SendLoraData.buffer[size++] = g_msgcount;

  g_SendLoraData.buffsize = size;
  SendData();

  g_msgcount++;
  if (g_EventType == EventType::LoraDataReceived) // check if we received some data, and if so, fire things off
  {
    SERIAL_LOG("Running handleReceivedMesage from DoPeriodicUpdate()");
    handleReceivedMessage();
  }
};

void loop()
{
  SERIAL_LOG("LOOP()");
  if (xSemaphoreTake(g_taskEvent, portMAX_DELAY) == pdTRUE)
  {
    SERIAL_LOG("Running loop for EventType: %d", g_EventType);

#ifndef MAX_SAVE
    digitalWrite(LED_GREEN, HIGH); // indicate we're doing stuff
#endif
    switch (g_EventType)
    {
    case EventType::LoraDataReceived:
      handleReceivedMessage();
      break;
    case EventType::Timer:
      doPeriodicUpdate();
      break;
    case EventType::None:
    default:
      SERIAL_LOG("In loop, but without correct g_EventType")
      break;
    };

#ifndef MAX_SAVE
    digitalWrite(LED_GREEN, LOW);
#endif
  }
  xSemaphoreTake(g_taskEvent, 10);
}
