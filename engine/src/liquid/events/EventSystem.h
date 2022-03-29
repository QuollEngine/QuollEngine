#pragma once

#include "EventPool.h"
#include "liquid/physics/CollisionEvent.h"

namespace liquid {

class EventSystem {
  using CollisionPool = EventPool<CollisionEvent, CollisionObject>;

public:
  EventSystem() = default;

  ~EventSystem() = default;

  EventSystem(const EventSystem &) = delete;
  EventSystem &operator=(const EventSystem &) = delete;
  EventSystem(EventSystem &&) = delete;
  EventSystem &operator=(EventSystem &&) = delete;

  /**
   * @brief Add observer for collision events
   *
   * @param type Collision event type
   * @param observer Collision event observer
   * @return Observer Id
   */
  inline EventObserverId observe(CollisionEvent type,
                                 const CollisionPool::EventObserver &observer) {
    return mCollisionPool.observe(type, observer);
  }

  /**
   * @brief Remove collision event observer
   *
   * @param type Collision event type
   * @param id Observer Id
   */
  inline void removeObserver(CollisionEvent type, EventObserverId id) {
    mCollisionPool.removeObserver(type, id);
  }

  /**
   * @brief Dispatch collision event
   *
   * @param type Collision event
   * @param collision Collision object
   */
  inline void dispatch(CollisionEvent type, const CollisionObject &collision) {
    mCollisionPool.dispatch(type, collision);
  }

  /**
   * @brief Poll events
   */
  inline void poll() {
    LIQUID_PROFILE_EVENT("EventSystem::poll");
    mCollisionPool.poll();
  }

private:
  CollisionPool mCollisionPool;
};

} // namespace liquid
