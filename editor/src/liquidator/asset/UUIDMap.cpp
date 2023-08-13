#include "liquid/core/Base.h"
#include "UUIDMap.h"

namespace liquid::editor {

String getUUIDFromMap(const UUIDMap &uuids, const String &key) {
  auto it = uuids.find(key);
  if (it != uuids.end()) {
    return it->second;
  }

  return "";
}

} // namespace liquid::editor
