#pragma once

#include "AssetData.h"

namespace liquid {

/**
 * @brief Asset map
 *
 * Store all the assets of a specific type
 *
 * @tparam THandle Asset handle type
 * @tparam TData Asset data type
 */
template <class THandle, class TData> class AssetMap {
public:
  /**
   * @brief Add asset
   *
   * @param data Asset data
   * @return New asset handle
   */
  THandle addAsset(const AssetData<TData> &data) {
    auto handle = getNewHandle();
    mAssets.insert_or_assign(handle, data);
    return handle;
  }

  /**
   * @brief Get asset
   *
   * @param handle Asset handle
   * @return Asset
   */
  const AssetData<TData> &getAsset(THandle handle) const {
    return mAssets.at(handle);
  }

  /**
   * @brief Get all assets
   *
   * @return List of all assets
   */
  inline const std::unordered_map<THandle, AssetData<TData>> &
  getAssets() const {
    return mAssets;
  }

  /**
   * @brief Get all assets
   *
   * @return List of all assets
   */
  inline std::unordered_map<THandle, AssetData<TData>> &getAssets() {
    return mAssets;
  }

  /**
   * @brief Check if asset exists
   *
   * @retval true Asset exists
   * @retval false Asset does not exist
   */
  inline bool hasAsset(THandle handle) const {
    return mAssets.find(handle) != mAssets.end();
  }

private:
  THandle getNewHandle() {
    THandle handle = mLastHandle;
    mLastHandle = THandle{static_cast<uint32_t>(mLastHandle) + 1};
    return handle;
  }

private:
  std::unordered_map<THandle, AssetData<TData>> mAssets;
  THandle mLastHandle{1};
};

} // namespace liquid
