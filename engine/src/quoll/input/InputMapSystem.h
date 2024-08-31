#pragma once

#include "quoll/input/InputMap.h"

namespace quoll {

class InputDeviceManager;
struct SystemView;

class InputMapSystem {
public:
  InputMapSystem(InputDeviceManager &deviceManager);

  void update(SystemView &view);

private:
  InputMap createInputMap(InputMapAsset &asset, usize defaultScheme);

private:
  InputDeviceManager &mDeviceManager;
};

} // namespace quoll
