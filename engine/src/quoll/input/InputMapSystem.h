#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"

#include "InputDeviceManager.h"

namespace quoll {

/**
 * @brief Input map system
 *
 * Maps input events to commands
 */
class InputMapSystem {
public:
  /**
   * @brief Create input map system
   *
   * @param deviceManager Device manager
   * @param assetRegistry Asset registry
   */
  InputMapSystem(InputDeviceManager &deviceManager,
                 AssetRegistry &assetRegistry);

  /**
   * @brief Update
   *
   * @param entityDatabase Entity database
   */
  void update(EntityDatabase &entityDatabase);

private:
  InputMap createInputMap(InputMapAsset &asset, usize defaultScheme);

private:
  InputDeviceManager &mDeviceManager;
  AssetRegistry &mAssetRegistry;
};

} // namespace quoll
