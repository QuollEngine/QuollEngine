#pragma once

#include "BufferDescription.h"
#include "TextureDescription.h"
#include "RenderHandle.h"

namespace liquid::experimental {

/**
 * @brief Registry map
 *
 * Stores descriptions in a hash map
 * and provides information about newly
 * added, updated, and deleted descriptions
 *
 * @tparam THandle Resource Handle type
 * @tparam TDescription Description type
 */
template <class THandle, class TDescription> class ResourceRegistryMap {
  using HandleList = std::vector<THandle>;

public:
  /**
   * @brief Add description
   *
   * @param description Description
   * @return Resource handle
   */
  THandle addDescription(const TDescription &description) {
    THandle newHandle = mLastHandle++;
    mDescriptions.insert({newHandle, description});
    mDirtyCreates.push_back(newHandle);

    return newHandle;
  }

  /**
   * @brief Update description
   *
   * @param handle Resource handle
   * @param description Description
   */
  inline void updateDescription(THandle handle,
                                const TDescription &description) {
    mDescriptions.at(handle) = description;
    mDirtyUpdates.push_back(handle);
  }

  /**
   * @brief Delete description
   *
   * @param handle Resource handle
   */
  inline void deleteDescription(THandle handle) {
    mDescriptions.erase(handle);
    mDirtyDeletes.push_back(handle);
  }

  /**
   * @brief Get description
   *
   * @param handle Resource handle
   * @return Description
   */
  inline const TDescription &getDescription(THandle handle) const {
    return mDescriptions.at(handle);
  }

  /**
   * @brief Check if resource description exists
   *
   * @param handle Resource handle
   * @retval true Description exists
   * @retval false Description does not exist
   */
  inline bool hasDescription(THandle handle) const {
    return mDescriptions.find(handle) != mDescriptions.end();
  }

  /**
   * @brief Get newly added resources
   *
   * @return List of newly added resources
   */
  inline const HandleList &getDirtyCreates() const { return mDirtyCreates; }

  /**
   * @brief Get newly updated resources
   *
   * @return List of newly updated resources
   */
  inline const HandleList &getDirtyUpdates() const { return mDirtyUpdates; }

  /**
   * @brief Get newly deleted resources
   *
   * @return List of newly deleted resources
   */
  inline const HandleList &getDirtyDeletes() const { return mDirtyDeletes; }

  /**
   * @brief Clear dirty creates
   */
  inline void clearDirtyCreates() { mDirtyCreates.clear(); }

  /**
   * @brief Clear dirty deletes
   */
  inline void clearDirtyDeletes() { mDirtyDeletes.clear(); }

  /**
   * @brief Clear dirty updates
   */
  inline void clearDirtyUpdates() { mDirtyUpdates.clear(); }

private:
  std::unordered_map<THandle, TDescription> mDescriptions;
  std::vector<THandle> mDirtyCreates;
  std::vector<THandle> mDirtyUpdates;
  std::vector<THandle> mDirtyDeletes;

  // ZERO means undefined
  THandle mLastHandle = 1;
};

class ResourceRegistry {
public:
  /**
   * @brief Add buffer
   *
   * @param description Buffer description
   * @return Buffer handle
   */
  BufferHandle addBuffer(const BufferDescription &description);

  /**
   * @brief Delete buffer
   *
   * @param handle Buffer handle
   */
  void deleteBuffer(BufferHandle handle);

  /**
   * @brief Update buffer
   *
   * @param handle Buffer handle
   * @param description Buffer description
   */
  void updateBuffer(BufferHandle handle, const BufferDescription &description);

  /**
   * @brief Get buffer map
   *
   * @return Buffer map
   */
  inline ResourceRegistryMap<BufferHandle, BufferDescription> &getBufferMap() {
    return mBuffers;
  }

  /**
   * @brief Add texture
   *
   * @param description Texture description
   * @return Texture handle
   */
  TextureHandle addTexture(const TextureDescription &description);

  /**
   * @brief Remove texture
   *
   * @param handle Texture handle
   */
  void deleteTexture(TextureHandle handle);

  /**
   * @brief Get texture map
   *
   * @return Texture map
   */
  inline ResourceRegistryMap<TextureHandle, TextureDescription> &
  getTextureMap() {
    return mTextures;
  }

private:
  ResourceRegistryMap<BufferHandle, BufferDescription> mBuffers;
  ResourceRegistryMap<TextureHandle, TextureDescription> mTextures;
};

} // namespace liquid::experimental