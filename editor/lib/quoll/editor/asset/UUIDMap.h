#pragma once

namespace quoll::editor {

using UUIDMap = std::unordered_map<String, Uuid>;

Uuid getOrCreateUuidFromMap(const UUIDMap &uuids, const String &key);

} // namespace quoll::editor
