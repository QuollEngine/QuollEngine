#pragma once

#include "quoll/core/SparseSet.h"
#include "Events.h"

namespace qui {

class EventManager;

class EventHolder {
public:
  constexpr EventHolder() = default;
  constexpr EventHolder(EventManager *manager) : mManager(manager) {}
  constexpr EventHolder(EventHolder &&rhs) noexcept
      : mManager(rhs.mManager),
        mMouseClickHandles(std::move(rhs.mMouseClickHandles)),
        mMouseDownHandles(std::move(rhs.mMouseDownHandles)),
        mMouseUpHandles(std::move(rhs.mMouseUpHandles)),
        mMouseMoveHandles(std::move(rhs.mMouseMoveHandles)),
        mMouseWheelHandles(std::move(rhs.mMouseWheelHandles)) {}

  constexpr EventHolder &operator=(EventHolder &&rhs) noexcept {
    mManager = rhs.mManager;
    mMouseClickHandles = std::move(rhs.mMouseClickHandles);
    mMouseDownHandles = std::move(rhs.mMouseDownHandles);
    mMouseUpHandles = std::move(rhs.mMouseUpHandles);
    mMouseMoveHandles = std::move(rhs.mMouseMoveHandles);
    mMouseWheelHandles = std::move(rhs.mMouseWheelHandles);

    return *this;
  }

  constexpr EventHolder(const EventHolder &rhs) {
    mManager = rhs.mManager;
    mMouseClickHandles = rhs.mMouseClickHandles;
    mMouseDownHandles = rhs.mMouseDownHandles;
    mMouseUpHandles = rhs.mMouseUpHandles;
    mMouseMoveHandles = rhs.mMouseMoveHandles;
    mMouseWheelHandles = rhs.mMouseWheelHandles;
  };

  constexpr EventHolder &operator=(const EventHolder &) = delete;
  ~EventHolder();

  void registerMouseClickHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseDownHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseUpHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseMoveHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseWheelHandler(EventHandler<MouseWheelEvent> &&handler);

private:
  EventManager *mManager{nullptr};

  std::vector<usize> mMouseClickHandles;
  std::vector<usize> mMouseDownHandles;
  std::vector<usize> mMouseUpHandles;
  std::vector<usize> mMouseMoveHandles;
  std::vector<usize> mMouseWheelHandles;
};

class EventManager {
  friend class EventHolder;

  template <typename TEvent>
  using HandlerSet = quoll::SparseSet<EventHandler<TEvent>>;

public:
  inline HandlerSet<MouseEvent> &getMouseClickHandlers() {
    return mMouseClickHandlers;
  }

  inline HandlerSet<MouseEvent> &getMouseDownHandlers() {
    return mMouseDownHandlers;
  }

  inline HandlerSet<MouseEvent> &getMouseUpHandlers() {
    return mMouseUpHandlers;
  }

  inline HandlerSet<MouseEvent> &getMouseMoveHandlers() {
    return mMouseMoveHandlers;
  }

  inline HandlerSet<MouseWheelEvent> &getMouseWheelHandlers() {
    return mMouseWheelHandlers;
  }

  inline EventHolder createEventHolder() { return EventHolder(this); }

private:
  HandlerSet<MouseEvent> mMouseClickHandlers;
  HandlerSet<MouseEvent> mMouseDownHandlers;
  HandlerSet<MouseEvent> mMouseUpHandlers;
  HandlerSet<MouseEvent> mMouseMoveHandlers;
  HandlerSet<MouseWheelEvent> mMouseWheelHandlers;
};

} // namespace qui
