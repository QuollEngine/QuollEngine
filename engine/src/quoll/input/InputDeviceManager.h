#pragma once

#include "InputDevice.h"

namespace quoll {

class InputDeviceManager : NoCopyMove {
public:
  InputDeviceManager() = default;
  ~InputDeviceManager() = default;

  void addDevice(InputDevice device);

  void removeDevice(InputDeviceType type, u32 index);

  inline const std::vector<InputDevice> &getDevices() const { return mDevices; }

private:
  std::vector<InputDevice> mDevices;
};

} // namespace quoll
