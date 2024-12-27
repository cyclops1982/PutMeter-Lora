#pragma once
#include <Arduino.h>
#include <serialhelper.h>
#include <lorahelper.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
using namespace Adafruit_LittleFS_Namespace;

// These config settings can be updated remotely.
enum ConfigType
{
    SleepTime0 = 0x10,
    TankDepth = 0x20,
    LORA_TXPower = 0x40,
    LORA_DataRate = 0x41,
    LORA_ADREnabled = 0x42,
    LORA_RequireConfirmation = 0x43,
    LORA_DevEUI = 0x48,
    LORA_NodeAppKey = 0x49,
    Restart = 0xF0,
    ClearConfig = 0xF1,
    SaveConfig = 0xF2
};

struct ConfigOption
{
    const char *name;
    ConfigType configType;
    size_t sizeOfOption;
    void *value;
    void (*setfunc)(const ConfigOption *opt, uint8_t *arr);
};

struct ConfigurationParameters
{
   // New device has AAA values
    uint8_t _loraDevEUI[8] = {0xAC, 0x1F, 0x10, 0xFF, 0xAA, 0xAA, 0xAA, 0xAA};
    uint8_t _loraNodeAppKey[16] = {0x66, 0x7b, 0x90, 0x71, 0xa1, 0x72, 0x18, 0xd4, 0xcd, 0xb2, 0x13, 0x04, 0x3f, 0xAA, 0xAA, 0xAA};

    // Config settings
    uint16_t _sleeptime0 = 300; // in seconds
    uint16_t _tankDepth = 200; // in CM

    void *_dummy;

    int8_t _loraDataRate = DR_2;
    int8_t _loraTXPower = TX_POWER_2;
    bool _loraADREnabled = false;
    bool _loraRequireConfirmation = false;
    uint8_t _loraNodeAppEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    static void SetUint8x8(const ConfigOption *option, uint8_t *arr);
    static void SetUint8x16(const ConfigOption *option, uint8_t *arr);
    static void SetUint32(const ConfigOption *option, uint8_t *arr);
    static void SetUint16(const ConfigOption *option, uint8_t *arr);
    static void SetUint8(const ConfigOption *option, uint8_t *arr);
    static void SetInt8(const ConfigOption *option, uint8_t *arr);
    static void SetBool(const ConfigOption *option, uint8_t *arr);
    static void Restart(const ConfigOption *option, uint8_t *arr);
    static void DoNothing(const ConfigOption *option, uint8_t *arr);
};

class ConfigHelper
{

public:
    uint32_t GetSleepTime0InSeconds() { return configvalues._sleeptime0; }
    uint16_t GetTankDepth() { return configvalues._tankDepth; }
    uint8_t GetLoraTXPower() { return configvalues._loraTXPower; }
    uint8_t GetLoraDataRate() { return configvalues._loraDataRate; }
    bool GetLoraADREnabled() { return configvalues._loraADREnabled; }
    uint8_t *GetLoraDevEUI() { return configvalues._loraDevEUI; }
    uint8_t *GetLoraNodeAppEUI() { return configvalues._loraNodeAppEUI; }
    uint8_t *GetLoraAppKey() { return configvalues._loraNodeAppKey; }
    bool GetLoraRequireConfirmation() { return configvalues._loraRequireConfirmation; }

    ConfigOption *GetConfigs(size_t *size);
    void SetConfig(uint8_t *arr, uint8_t length);
    bool InitConfig();

private:
    /*
        We've though quite a bit about versioning, as the config is just the ConfigurationParameter struct writen to a file.
        Now, we could introduce versioning on the ConfigType and ConfigurationParameter so that we can write upgrade code.
        We could read the old file, load it and copy the values to the new configuration option and save that (into a new filename).
        This is all cool and would mean we can 'upgrade' without having to reset parameters.
        In reality, we can only upgrade the firmware when the device is connected to USB, and thus the re-programming becomes a fairly easy.
        In short: we will never really write 'upgrade code' to move from V1 to V2.  It is however practical to have the filename different,
        as we can then change that to ignore the 'old' settings.
        */
    const char *CONFIG_NAME = "config_w1.bin";
    const char *OLD_CONFIG_NAMES[1] = {
        "config_w0.bin"};
    bool SaveConfig();
    void ResetConfig();

    ConfigurationParameters configvalues;

    ConfigOption configs[11] = {
        {"Sleep time between readings", ConfigType::SleepTime0, sizeof(ConfigurationParameters::_sleeptime0), &configvalues._sleeptime0, ConfigurationParameters::SetUint16},
        {"Tank depth", ConfigType::TankDepth, sizeof(ConfigurationParameters::_tankDepth), &configvalues._tankDepth, ConfigurationParameters::SetUint16},
        {"LoraWAN - TX Power", ConfigType::LORA_TXPower, sizeof(ConfigurationParameters::_loraTXPower), &configvalues._loraTXPower, ConfigurationParameters::SetInt8},
        {"LoraWAN - DataRate", ConfigType::LORA_DataRate, sizeof(ConfigurationParameters::_loraDataRate), &configvalues._loraDataRate, ConfigurationParameters::SetInt8},
        {"LoraWAN - ADR Enabled", ConfigType::LORA_ADREnabled, sizeof(ConfigurationParameters::_loraADREnabled), &configvalues._loraADREnabled, ConfigurationParameters::SetBool},
        {"LoraWAN - Require confirmation message", ConfigType::LORA_RequireConfirmation, sizeof(ConfigurationParameters::_loraRequireConfirmation), &configvalues._loraRequireConfirmation, ConfigurationParameters::SetBool},
        {"LoraWAN - Dev EUI", ConfigType::LORA_DevEUI, sizeof(ConfigurationParameters::_loraDevEUI), &configvalues._loraDevEUI, ConfigurationParameters::SetUint8x8},
        {"LoraWAN - Node App key", ConfigType::LORA_NodeAppKey, sizeof(ConfigurationParameters::_loraNodeAppKey), &configvalues._loraNodeAppKey, ConfigurationParameters::SetUint8x16},
        {"Restart Device", ConfigType::Restart, sizeof(ConfigurationParameters::_dummy), &configvalues._dummy, ConfigurationParameters::Restart},
        {"Clear Config", ConfigType::ClearConfig, sizeof(ConfigurationParameters::_dummy), &configvalues._dummy, ConfigurationParameters::DoNothing},
        {"Save Config", ConfigType::SaveConfig, sizeof(ConfigurationParameters::_dummy), &configvalues._dummy, ConfigurationParameters::DoNothing}};
};
