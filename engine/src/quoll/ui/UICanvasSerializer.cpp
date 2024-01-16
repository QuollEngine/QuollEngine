#include "quoll/core/Base.h"
#include "UICanvas.h"
#include "UICanvasSerializer.h"

namespace quoll {

void UICanvasSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<UICanvas>(entity)) {
    node["uiCanvas"] = YAML::Node(YAML::NodeType::Map);
  }
}

void UICanvasSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity) {

  if (node["uiCanvas"] && node["uiCanvas"].IsMap()) {
    entityDatabase.set<UICanvas>(entity, {});
  }
}

} // namespace quoll
