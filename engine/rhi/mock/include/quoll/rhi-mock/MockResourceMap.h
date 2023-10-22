#pragma once

namespace quoll::rhi {

/**
 * @brief Map for mock resources
 *
 * @tparam THandle Handle type
 * @tparam TObject Object type
 */
template <class THandle, class TResource> class MockResourceMap {
public:
  /**
   * @brief Insert resource
   *
   * @param resource Resource
   * @return Resource handle
   */
  THandle insert(const TResource &resource) {
    auto handle = static_cast<THandle>(mIncrement);
    mResources.insert_or_assign(handle, std::move(resource));

    mIncrement++;
    return handle;
  }

  /**
   * @brief Insert resource
   *
   * @param resource Resource
   * @return Resource handle
   */
  THandle insert(TResource &&resource) {
    auto handle = static_cast<THandle>(mIncrement);
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);

    mIncrement++;
    return handle;
  }

  /**
   * @brief Insert resource
   *
   * @param resource Resource
   * @param handle Resource handle
   */
  void insert(const TResource &resource, THandle handle) {
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  /**
   * @brief Insert resource
   *
   * @param resource Resource
   * @param handle Resource handle
   */
  void insert(TResource &&resource, THandle handle) {
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  /**
   * @brief Insert resource
   *
   * @param handle Resource handle
   * @param resource Resource
   */
  void replace(THandle handle, TResource &&resource) {
    mResources.emplace(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  /**
   * @brief Get resource
   *
   * @param handle Resource handle
   * @return Resource
   */
  const TResource &at(THandle handle) const { return mResources.at(handle); }

  /**
   * @brief Clear resource
   */
  void clear() {
    mResources.clear();
    mEmplacements.clear();
    mIncrement = 1;
  }

  /**
   * @brief Remove resource
   *
   * @param handle Resource handle
   */
  void erase(THandle handle) { mResources.erase(handle); }

  /**
   * @brief Check if registry exists
   *
   * @param handle Resource handle
   * @retval true Resource exists
   * @retval false Resources does not exist
   */
  bool exists(THandle handle) const {
    return mResources.find(handle) != mResources.end();
  }

  /**
   * @brief Get number of times the handle is emplaced
   *
   * @param handle Resource handle
   * @return Number of times the handle is emplaced
   */
  u32 getEmplaced(THandle handle) const { return mEmplacements.at(handle); }

private:
  void incrementEmplacement(THandle handle) {
    auto it = mEmplacements.find(handle);
    if (it != mEmplacements.end()) {
      mEmplacements.at(handle)++;
    } else {
      mEmplacements.insert({handle, 1});
    }
  }

private:
  u32 mIncrement = 1;
  std::unordered_map<THandle, TResource> mResources;
  std::unordered_map<THandle, u32> mEmplacements;
};

} // namespace quoll::rhi
