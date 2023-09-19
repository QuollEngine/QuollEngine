#include "quoll/core/Base.h"
#include "TestPhysicsBackend.h"

void TestPhysicsBackend::update(float dt,
                                quoll::EntityDatabase &entityDatabase) {}

void TestPhysicsBackend::cleanup(quoll::EntityDatabase &entityDatabase) {}

void TestPhysicsBackend::observeChanges(quoll::EntityDatabase &entityDatabase) {
}

bool TestPhysicsBackend::sweep(quoll::EntityDatabase &entityDatabase,
                               quoll::Entity entity, const glm::vec3 &direction,
                               float distance) {
  return mSweepValue;
}

void TestPhysicsBackend::setSweepValue(bool value) { mSweepValue = value; }
