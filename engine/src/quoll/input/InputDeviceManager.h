#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "InputDevice.h"

namespace quoll {

class InputDeviceManager {
public:
  InputDeviceManager() = default;
  ~InputDeviceManager() = default;

  InputDeviceManager(const InputDeviceManager &) = delete;
  InputDeviceManager &operator=(const InputDeviceManager &) = delete;
  InputDeviceManager(InputDeviceManager &&) = delete;
  InputDeviceManager &operator=(InputDeviceManager &&) = delete;

  void addDevice(InputDevice device);

  void removeDevice(InputDeviceType type, u32 index);

  inline const std::vector<InputDevice> &getDevices() const { return mDevices; }

private:
  std::vector<InputDevice> mDevices;
};

} // namespace quoll
