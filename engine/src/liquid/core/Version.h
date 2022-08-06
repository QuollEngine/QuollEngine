#pragma once

namespace liquid {

/**
 * @brief Create version
 *
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @param build Build number
 * @return Version uint
 */
constexpr inline uint64_t createVersion(uint8_t major, uint8_t minor = 0,
                                        uint8_t patch = 0, uint32_t build = 0) {
  return (static_cast<uint64_t>(major) << 56) |
         (static_cast<uint64_t>(minor) << 48) |
         (static_cast<uint64_t>(patch) << 40) | (static_cast<uint64_t>(build));
}

} // namespace liquid
