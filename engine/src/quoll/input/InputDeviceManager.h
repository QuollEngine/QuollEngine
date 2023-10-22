#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"

#include "InputDevice.h"

namespace quoll {

/**
 * @brief Input device manager
 *
 * Stores all input devices visible from engine
 */
class InputDeviceManager {
public:
  InputDeviceManager() = default;
  ~InputDeviceManager() = default;

  InputDeviceManager(const InputDeviceManager &) = delete;
  InputDeviceManager &operator=(const InputDeviceManager &) = delete;
  InputDeviceManager(InputDeviceManager &&) = delete;
  InputDeviceManager &operator=(InputDeviceManager &&) = delete;

  /**
   * @brief Add input device
   *
   * @param device Device
   */
  void addDevice(InputDevice device);

  /**
   * @brief Remove input  device
   *
   * @param type Input device type
   * @param index Input device index
   */
  void removeDevice(InputDeviceType type, u32 index);

  /**
   * @brief Get input devices
   *
   * @return Input devices
   */
  inline const std::vector<InputDevice> &getDevices() const { return mDevices; }

private:
  std::vector<InputDevice> mDevices;
};

} // namespace quoll
