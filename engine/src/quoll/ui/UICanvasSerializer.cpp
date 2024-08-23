#include "quoll/core/Base.h"
#include "UICanvas.h"
#include "UICanvasSerializer.h"

namespace quoll {

void UICanvasSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entity.has<UICanvas>()) {
    node["uiCanvas"] = YAML::Node(YAML::NodeType::Map);
  }
}

void UICanvasSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity) {

  if (node["uiCanvas"] && node["uiCanvas"].IsMap()) {
    entity.set<UICanvas>({});
  }
}

} // namespace quoll
