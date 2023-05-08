#pragma once

namespace liquid::rhi {

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
  }

  /**
   * @brief Insert resource
   *
   * @param resource Resource
   * @param handle Resource handle
   */
  void insert(TResource &&resource, THandle handle) {
    mResources.insert_or_assign(handle, std::move(resource));
  }

  /**
   * @brief Insert resource
   *
   * @param handle Resource handle
   * @param resource Resource
   */
  void replace(THandle handle, TResource &&resource) {
    mResources.emplace(handle, std::move(resource));
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

private:
  uint32_t mIncrement = 1;
  std::unordered_map<THandle, TResource> mResources;
};

} // namespace liquid::rhi
