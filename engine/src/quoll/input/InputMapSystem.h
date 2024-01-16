#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/input/InputMap.h"

#include "InputDeviceManager.h"

namespace quoll {

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
