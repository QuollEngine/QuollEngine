#pragma once

#include "quoll/physics/PhysicsBackend.h"

class TestPhysicsBackend : public quoll::PhysicsBackend {
public:
  void update(f32 dt, quoll::SystemView &view) override;

  void cleanup(quoll::SystemView &view) override;

  void createSystemViewData(quoll::SystemView &view) override;

  bool sweep(quoll::EntityDatabase &entityDatabase, quoll::Entity entity,
             const glm::vec3 &direction, f32 distance,
             quoll::CollisionHit &hit) override;

  void setSweepValue(bool value);

  void setSweepHitData(quoll::CollisionHit hit);

  constexpr quoll::PhysicsSignals &getSignals() override { return mSignals; }

  constexpr quoll::debug::DebugPanel *getDebugPanel() override {
    return nullptr;
  }

private:
  bool mSweepValue = true;
  quoll::CollisionHit mHit;
  quoll::PhysicsSignals mSignals;
};
