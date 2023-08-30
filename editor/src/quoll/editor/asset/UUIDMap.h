#pragma once

namespace quoll::editor {

using UUIDMap = std::unordered_map<String, Uuid>;

/**
 * @brief Get uuid from map
 *
 * @param uuids Uuid map
 * @param key Uuid map key
 * @return Uuid
 */
Uuid getOrCreateUuidFromMap(const UUIDMap &uuids, const String &key);

} // namespace quoll::editor
