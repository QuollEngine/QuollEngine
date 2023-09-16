#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "InputDeviceManager.h"

#include "KeyMappings.h"

#include <GLFW/glfw3.h>

namespace quoll {

void InputDeviceManager::addDevice(InputDevice device) {
  mDevices.push_back(device);
  Engine::getUserLogger().info()
      << "Device \"" << device.name << "\" connected";
}

void InputDeviceManager::removeDevice(InputDeviceType type, uint32_t index) {
  auto it =
      std::find_if(mDevices.begin(), mDevices.end(),
                   [type, index](const InputDevice &existing) {
                     return existing.type == type && existing.index == index;
                   });

  if (it == mDevices.end()) {
    return;
  }

  auto name = it->name;
  mDevices.erase(it);
  Engine::getUserLogger().info() << "Device \"" << name << "\" disconnected";
}

} // namespace quoll
