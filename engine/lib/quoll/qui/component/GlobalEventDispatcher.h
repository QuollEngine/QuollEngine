#pragma once

#include "quoll/core/SparseSet.h"
#include "Events.h"

namespace qui {

template <typename TEvent> class HandlerSet {
public:
  using Handlers = quoll::SparseSet<EventHandler<TEvent>>;

public:
  constexpr Handlers &getHandlers() { return mHandlers; }
  constexpr const Handlers &getHandlers() const { return mHandlers; }

  constexpr usize insert(EventHandler<TEvent> &&handler) {
    auto handle = mHandlers.insert(std::move(handler));
    mRefCounts.insert({handle, 0});
    return handle;
  }

  inline void take(usize handle) {
    auto it = mRefCounts.find(handle);
    it->second = it->second + 1;
  }

  inline void release(usize handle) {
    auto it = mRefCounts.find(handle);
    QuollAssert(it->second > 0, "Asset cannot have reference count of zero");

    it->second = it->second - 1;

    if (it->second == 0) {
      mHandlers.erase(handle);
    }
  }

private:
  quoll::SparseSet<EventHandler<TEvent>> mHandlers;
  std::unordered_map<usize, usize> mRefCounts;
};

template <typename TEvent> class EventHandle {
  using Set = HandlerSet<TEvent>;

public:
  constexpr EventHandle() = default;
  constexpr EventHandle(Set *handlers, usize id)
      : mHandlers(handlers), mId(id) {
    mHandlers->take(mId);
  }

  constexpr ~EventHandle() {
    if (mHandlers) {
      mHandlers->release(mId);
    }
  }

  constexpr EventHandle(EventHandle &&rhs) noexcept
      : mHandlers(rhs.mHandlers), mId(rhs.mId) {
    rhs.mHandlers = nullptr;
    rhs.mId = Set::Handlers::Empty;
  }

  constexpr EventHandle &operator=(EventHandle &&rhs) noexcept {
    if (mHandlers) {
      mHandlers->release(mId);
    }
    mHandlers = rhs.mHandlers;
    mId = rhs.mId;

    rhs.mHandlers = nullptr;
    rhs.mId = Set::Handlers::Empty;

    return *this;
  }

  constexpr EventHandle(const EventHandle &rhs)
      : mHandlers(rhs.mHandlers), mId(rhs.mId) {}

  constexpr EventHandle &operator=(const EventHandle &rhs) {
    if (mHandlers) {
      mHandlers->release(mId);
    }

    mHandlers = rhs.mHandlers;
    mId = rhs.mId;

    if (mHandlers) {
      mHandlers->take(mId);
    }

    return *this;
  }

private:
  Set *mHandlers = nullptr;
  usize mId = Set::Handlers::Empty;
};

class GlobalEventDispatcher {
  friend class EventHolder;

public:
  constexpr const auto &getMouseClickHandlers() const {
    return mMouseClickHandlers.getHandlers();
  }

  constexpr const auto &getMouseDownHandlers() const {
    return mMouseDownHandlers.getHandlers();
  }

  constexpr const auto &getMouseUpHandlers() const {
    return mMouseUpHandlers.getHandlers();
  }

  constexpr const auto &getMouseMoveHandlers() const {
    return mMouseMoveHandlers.getHandlers();
  }

  constexpr const auto &getMouseWheelHandlers() const {
    return mMouseWheelHandlers.getHandlers();
  }

  constexpr auto registerMouseClickHandler(EventHandler<MouseEvent> &&handler) {
    return EventHandle(&mMouseClickHandlers,
                       mMouseClickHandlers.insert(std::move(handler)));
  }

  constexpr auto registerMouseDownHandler(EventHandler<MouseEvent> &&handler) {
    return EventHandle(&mMouseDownHandlers,
                       mMouseDownHandlers.insert(std::move(handler)));
  }

  constexpr auto registerMouseUpHandler(EventHandler<MouseEvent> &&handler) {
    return EventHandle(&mMouseUpHandlers,
                       mMouseUpHandlers.insert(std::move(handler)));
  }

  constexpr auto registerMouseMoveHandler(EventHandler<MouseEvent> &&handler) {
    return EventHandle(&mMouseMoveHandlers,
                       mMouseMoveHandlers.insert(std::move(handler)));
  }

  constexpr auto
  registerMouseWheelHandler(EventHandler<MouseWheelEvent> &&handler) {
    return EventHandle(&mMouseWheelHandlers,
                       mMouseWheelHandlers.insert(std::move(handler)));
  }

private:
  HandlerSet<MouseEvent> mMouseClickHandlers;
  HandlerSet<MouseEvent> mMouseDownHandlers;
  HandlerSet<MouseEvent> mMouseUpHandlers;
  HandlerSet<MouseEvent> mMouseMoveHandlers;
  HandlerSet<MouseWheelEvent> mMouseWheelHandlers;
};

} // namespace qui
