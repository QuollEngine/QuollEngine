#pragma once

#include "quoll/input/InputMap.h"

namespace quoll {

class AssetRegistry;
class InputDeviceManager;
class EntityDatabase;

class InputMapSystem {
public:
  InputMapSystem(InputDeviceManager &deviceManager,
                 AssetRegistry &assetRegistry);

  void update(EntityDatabase &entityDatabase);

private:
  InputMap createInputMap(InputMapAsset &asset, usize defaultScheme);

private:
  InputDeviceManager &mDeviceManager;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
