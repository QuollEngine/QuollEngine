#pragma once

namespace quoll {

struct SystemView;

class SkeletonUpdater {
public:
  void createSystemViewData(SystemView &view);

  void update(SystemView &view);

private:
  void updateSkeletons(SystemView &view);

  void updateDebugBones(SystemView &view);
};

} // namespace quoll
