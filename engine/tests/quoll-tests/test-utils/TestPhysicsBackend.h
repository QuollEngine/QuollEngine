#pragma once

#include "quoll/physics/PhysicsBackend.h"

class TestPhysicsBackend : public quoll::PhysicsBackend {
public:
  void update(float dt, quoll::EntityDatabase &entityDatabase) override;

  void cleanup(quoll::EntityDatabase &entityDatabase) override;

  void observeChanges(quoll::EntityDatabase &entityDatabase) override;
};
