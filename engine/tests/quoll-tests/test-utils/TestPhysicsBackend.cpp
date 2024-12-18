#include "quoll/core/Base.h"
#include "TestPhysicsBackend.h"

void TestPhysicsBackend::update(f32 dt, quoll::SystemView &view) {}

void TestPhysicsBackend::cleanup(quoll::SystemView &view) {}

void TestPhysicsBackend::createSystemViewData(quoll::SystemView &view) {}

bool TestPhysicsBackend::sweep(quoll::EntityDatabase &entityDatabase,
                               quoll::Entity entity, const glm::vec3 &direction,
                               f32 distance, quoll::CollisionHit &hit) {
  if (mSweepValue) {
    hit.normal = mHit.normal;
    hit.distance = mHit.distance;
    hit.entity = mHit.entity;
  }

  return mSweepValue;
}

void TestPhysicsBackend::setSweepValue(bool value) { mSweepValue = value; }

void TestPhysicsBackend::setSweepHitData(quoll::CollisionHit hit) {
  mHit = hit;
}
