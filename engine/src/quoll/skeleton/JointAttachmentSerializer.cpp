#include "quoll/core/Base.h"

#include "JointAttachment.h"
#include "JointAttachmentSerializer.h"

namespace quoll {

void JointAttachmentSerializer::serialize(YAML::Node &node,
                                          EntityDatabase &entityDatabase,
                                          Entity entity) {
  if (entityDatabase.has<JointAttachment>(entity)) {
    auto joint = entityDatabase.get<JointAttachment>(entity).joint;
    if (joint >= 0) {
      node["jointAttachment"]["joint"] = joint;
    }
  }
}

void JointAttachmentSerializer::deserialize(const YAML::Node &node,
                                            EntityDatabase &entityDatabase,
                                            Entity entity) {
  if (node["jointAttachment"] && node["jointAttachment"].IsMap()) {
    auto joint = node["jointAttachment"]["joint"].as<i16>(-1);
    if (joint >= 0 && joint < std::numeric_limits<u8>::max()) {
      JointAttachment attachment{joint};
      entityDatabase.set(entity, attachment);
    }
  }
}

} // namespace quoll
