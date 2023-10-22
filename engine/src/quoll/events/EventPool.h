#pragma once

#include "quoll/core/RingBuffer.h"
#include "quoll/core/SwappableVector.h"
#include "quoll/core/SparseSet.h"
#include "EventObserver.h"

namespace quoll {

/**
 * @brief Event pool
 *
 * Stores and manages queues and
 * observers for events of a specific
 * type.
 *
 * @tparam TEvent Event type
 * @tparam TData Event data type
 * @tparam Size Pool size
 */
template <class TEvent, class TData> class EventPool {
  struct Event {
    TEvent type;
    TData data;
  };

public:
  /**
   * Event observer
   */
  using EventObserver = std::function<void(const TData &data)>;

  /**
   * Event data
   */
  using EventData = TData;

  /**
   * Default pool size
   */
  static constexpr usize DEFAULT_SIZE = 500;

public:
  /**
   * @brief Add observer
   *
   * @param type Event type
   * @param observer Event observer
   * @return Observer Id
   */
  EventObserverId observe(TEvent type, const EventObserver &observer) {
    if (mObservers.find(type) == mObservers.end()) {
      mObservers.insert({type, {}});
    }

    return mObservers.at(type).insert(observer);
  }

  /**
   * @brief Remove observer
   *
   * @param type Event type
   * @param observerId Observer Id
   */
  void removeObserver(TEvent type, EventObserverId observerId) {
    mObservers.at(type).erase(observerId);
  }

  /**
   * @brief Check if observer exists
   *
   * @param type Event type
   * @param observerId Observer Id
   * @retval true Observer exists
   * @retval Observer does not exist
   */
  bool hasObserver(TEvent type, EventObserverId observerId) {
    return mObservers.at(type).contains(observerId);
  }

  /**
   * @brief Dispatch event
   *
   * @param type Event type
   * @param data Event data
   */
  void dispatch(TEvent type, const TData &data) { mQueue.push({type, data}); }

  /**
   * @brief Poll events
   *
   * Traverse through the event queue
   * and call observer on all the events
   */
  void poll() {
    while (!mQueue.empty()) {
      auto &event = mQueue.front();

      if (mObservers.find(event.type) != mObservers.end()) {
        for (auto &observer : mObservers.at(event.type)) {
          observer(event.data);
        }
      }

      mQueue.pop();
    }
  }

  /**
   * @brief Get queue
   *
   * @return Event queue
   */
  inline const RingBuffer<Event> &getQueue() const { return mQueue; }

  /**
   * @brief Get observers for event type
   *
   * @param type Event type
   * @return Observers for event type
   */
  inline const SparseSet<EventObserver> &getObservers(TEvent type) const {
    QuollAssert(mObservers.find(type) != mObservers.end(),
                "No observers found for event");
    return mObservers.at(type);
  }

private:
  RingBuffer<Event> mQueue{DEFAULT_SIZE};
  std::map<TEvent, SparseSet<EventObserver>> mObservers;
};

} // namespace quoll
