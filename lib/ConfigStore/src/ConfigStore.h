#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "PlantTypes.h"

class ConfigStore {
public:
  bool begin();
  EmotionThresholds load() const;
  bool save(const EmotionThresholds& thresholds);

private:
  struct StoredConfig {
    uint32_t magic;
    EmotionThresholds thresholds;
    uint32_t crc;
  };

  static constexpr uint32_t kMagic = 0x504C4E54; // "PLNT"
  static constexpr int kEepromSize = 128;
  static constexpr int kBaseAddress = 0;

  static uint32_t crc32(const uint8_t* data, size_t length);
  static uint32_t payloadCrc(const StoredConfig& config);
};

