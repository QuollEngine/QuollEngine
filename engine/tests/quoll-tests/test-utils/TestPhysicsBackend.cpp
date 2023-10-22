#include "quoll/core/Base.h"
#include "TestPhysicsBackend.h"

void TestPhysicsBackend::update(f32 dt, quoll::EntityDatabase &entityDatabase) {
}

void TestPhysicsBackend::cleanup(quoll::EntityDatabase &entityDatabase) {}

void TestPhysicsBackend::observeChanges(quoll::EntityDatabase &entityDatabase) {
}

bool TestPhysicsBackend::sweep(quoll::EntityDatabase &entityDatabase,
                               quoll::Entity entity, const glm::vec3 &direction,
                               f32 distance, quoll::CollisionHit &hit) {
  if (mSweepValue) {
    hit.normal = mHit.normal;
  }

  return mSweepValue;
}

void TestPhysicsBackend::setSweepValue(bool value) { mSweepValue = value; }

void TestPhysicsBackend::setSweepHitData(quoll::CollisionHit hit) {
  mHit = hit;
}
