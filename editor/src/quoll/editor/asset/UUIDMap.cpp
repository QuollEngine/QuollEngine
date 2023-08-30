#include "quoll/core/Base.h"
#include "UUIDMap.h"

namespace quoll::editor {

Uuid getOrCreateUuidFromMap(const UUIDMap &uuids, const String &key) {
  auto it = uuids.find(key);
  if (it != uuids.end()) {
    return it->second;
  }

  return Uuid::generate();
}

} // namespace quoll::editor
