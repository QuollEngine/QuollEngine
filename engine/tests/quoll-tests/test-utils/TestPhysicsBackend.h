#pragma once

#include "quoll/physics/PhysicsBackend.h"

class TestPhysicsBackend : public quoll::PhysicsBackend {
public:
  void update(float dt, quoll::EntityDatabase &entityDatabase) override;

  void cleanup(quoll::EntityDatabase &entityDatabase) override;

  void observeChanges(quoll::EntityDatabase &entityDatabase) override;

  bool sweep(quoll::EntityDatabase &entityDatabase, quoll::Entity entity,
             const glm::vec3 &direction, float distance) override;

  void setSweepValue(bool value);

private:
  bool mSweepValue = true;
};
