#pragma once

#include "quoll/profiler/DebugPanel.h"
#include <PxPhysicsAPI.h>

namespace quoll::debug {

class PhysxDebugPanel : public DebugPanel {
public:
  void create(physx::PxFoundation *foundation);
  void release();

  void onRenderMenu() override;

  void onRender() override;

  constexpr physx::PxPvd *getPvd() { return mPvd; }

private:
  physx::PxPvd *mPvd = nullptr;
  bool mOpen = false;
  String mPvdHost{"127.0.0.1"};
  i32 mPvdPort = 5425;
};

} // namespace quoll::debug
