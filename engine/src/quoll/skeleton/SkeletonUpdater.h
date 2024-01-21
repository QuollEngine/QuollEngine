#pragma once

namespace quoll {

class EntityDatabase;

class SkeletonUpdater {
public:
  void update(EntityDatabase &entityDatabase);

private:
  void updateSkeletons(EntityDatabase &entityDatabase);

  void updateDebugBones(EntityDatabase &entityDatabase);
};

} // namespace quoll
