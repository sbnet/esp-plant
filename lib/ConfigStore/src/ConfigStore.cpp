#include "ConfigStore.h"

bool ConfigStore::begin() {
  EEPROM.begin(kEepromSize);
  return true;
}

EmotionThresholds ConfigStore::load() const {
  StoredConfig stored{};
  EEPROM.get(kBaseAddress, stored);

  if (stored.magic != kMagic) {
    return defaultEmotionThresholds();
  }

  if (stored.crc != payloadCrc(stored)) {
    return defaultEmotionThresholds();
  }

  if (!isValidEmotionThresholds(stored.thresholds)) {
    return defaultEmotionThresholds();
  }

  return stored.thresholds;
}

bool ConfigStore::save(const EmotionThresholds& thresholds) {
  if (!isValidEmotionThresholds(thresholds)) {
    return false;
  }

  StoredConfig stored{};
  stored.magic = kMagic;
  stored.thresholds = thresholds;
  stored.crc = payloadCrc(stored);

  EEPROM.put(kBaseAddress, stored);
  return EEPROM.commit();
}

uint32_t ConfigStore::payloadCrc(const StoredConfig& config) {
  uint32_t crc = 0;
  crc ^= crc32(reinterpret_cast<const uint8_t*>(&config.magic), sizeof(config.magic));
  crc ^= crc32(reinterpret_cast<const uint8_t*>(&config.thresholds), sizeof(config.thresholds));
  return crc;
}

uint32_t ConfigStore::crc32(const uint8_t* data, size_t length) {
  uint32_t crc = 0xFFFFFFFFu;

  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];
    for (int bit = 0; bit < 8; ++bit) {
      const uint32_t mask = -(crc & 1u);
      crc = (crc >> 1) ^ (0xEDB88320u & mask);
    }
  }

  return ~crc;
}

