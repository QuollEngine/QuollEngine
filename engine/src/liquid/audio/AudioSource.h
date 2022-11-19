#pragma once

namespace liquid {

/**
 * @brief Audio source component
 *
 * Stores all the sources associated with
 * an entity
 */
struct AudioSource {
  /**
   * @brief Audio source asset
   */
  AudioAssetHandle source = AudioAssetHandle::Invalid;
};

} // namespace liquid
