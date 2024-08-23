#pragma once

#include "quoll/input/InputMap.h"

namespace quoll {

class AssetRegistry;
class InputDeviceManager;
struct SystemView;

class InputMapSystem {
public:
  InputMapSystem(InputDeviceManager &deviceManager,
                 AssetRegistry &assetRegistry);

  void createSystemViewData(SystemView &view);

  void update(SystemView &view);

private:
  InputMap createInputMap(InputMapAsset &asset, usize defaultScheme);

private:
  InputDeviceManager &mDeviceManager;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
