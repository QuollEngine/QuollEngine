#pragma once

namespace quoll {

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
  AudioAssetHandle source = AudioAssetHandle::Null;
};

} // namespace quoll
