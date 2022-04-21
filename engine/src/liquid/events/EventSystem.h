#pragma once

#include "EventPool.h"
#include "EventDetails.h"

namespace liquid {

class EventSystem {
  /**
   * @brief Type "function" that provides event pool
   *
   * @tparam TEvent Event type
   * @return Event pool with intended type and data
   */
  template <class TEvent>
  using GetEventPool =
      EventPool<TEvent, typename event_detail::GetEventData<TEvent>::Data>;

  /**
   * @brief Tuple that wraps every element with EventPool
   *
   * @tparam ...TEvents
   * @example This tuple can be defined in the following way
   *          PoolTuple<CollisionEvent, MouseButtonEvent, ...> mPools;
   */
  template <class... TEvents>
  using PoolTuple = std::tuple<GetEventPool<TEvents>...>;

public:
  EventSystem() = default;

  ~EventSystem() = default;

  EventSystem(const EventSystem &) = delete;
  EventSystem &operator=(const EventSystem &) = delete;
  EventSystem(EventSystem &&) = delete;
  EventSystem &operator=(EventSystem &&) = delete;

  /**
   * @brief Add observer for specified event
   *
   * @tparam TEvent Event type num
   * @param type Event type
   * @param observer Event observer
   * @return Observer Id
   */
  template <class TEvent>
  inline EventObserverId
  observe(TEvent type,
          typename GetEventPool<TEvent>::EventObserver &&observer) {
    return std::get<GetEventPool<TEvent>>(mPools).observe(type, observer);
  }

  /**
   * @brief Remove collision event observer
   *
   * @tparam TEvent Event type enum
   * @param type Event type
   * @param id Observer Id
   */
  template <class TEvent>
  inline void removeObserver(TEvent type, EventObserverId id) {
    using CurrentPool = GetEventPool<TEvent>;
    std::get<CurrentPool>(mPools).removeObserver(type, id);
  }

  /**
   * @brief Dispatch collision event
   *
   * @tparam TEvent Event type enum
   * @param type Event type
   * @param data Event data
   */
  template <class TEvent>
  inline void dispatch(TEvent type,
                       const typename GetEventPool<TEvent>::EventData &data) {
    using CurrentPool = GetEventPool<TEvent>;
    std::get<CurrentPool>(mPools).dispatch(type, data);
  }

  /**
   * @brief Poll events
   */
  inline void poll() {
    LIQUID_PROFILE_EVENT("EventSystem::poll");
    poll<0, std::tuple_size<decltype(mPools)>::value>();
  }

private:
  /**
   * @brief Poll events
   *
   * Recursive template function
   *
   * @tparam Index Tuple index to loop
   * @tparam Size Tuple size
   */
  template <size_t Index, size_t Size> inline void poll() {
    if constexpr (Index < Size) {
      std::get<Index>(mPools).poll();
      poll<Index + 1, Size>();
    }
  }

private:
  PoolTuple<CollisionEvent, MouseButtonEvent, MouseCursorEvent,
            MouseScrollEvent, KeyboardEvent>
      mPools;
};

} // namespace liquid
