#pragma once

namespace liquid::editor {

using UUIDMap = std::unordered_map<String, String>;

/**
 * @brief Get uuid from map
 *
 * @param uuids Uuid map
 * @param key Uuid map key
 * @return Uuid or empty string
 */
String getUUIDFromMap(const UUIDMap &uuids, const String &key);

} // namespace liquid::editor
