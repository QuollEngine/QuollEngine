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
        mMouseMoveHandles(std::move(rhs.mMouseMoveHandles)) {}

  constexpr EventHolder &operator=(EventHolder &&rhs) noexcept {
    mManager = rhs.mManager;
    mMouseClickHandles = std::move(rhs.mMouseClickHandles);
    mMouseDownHandles = std::move(rhs.mMouseDownHandles);
    mMouseUpHandles = std::move(rhs.mMouseUpHandles);
    mMouseMoveHandles = std::move(rhs.mMouseMoveHandles);
    return *this;
  }

  constexpr EventHolder(const EventHolder &rhs) {
    mManager = rhs.mManager;
    mMouseClickHandles = rhs.mMouseClickHandles;
    mMouseDownHandles = rhs.mMouseDownHandles;
    mMouseUpHandles = rhs.mMouseUpHandles;
    mMouseMoveHandles = rhs.mMouseMoveHandles;
  };

  constexpr EventHolder &operator=(const EventHolder &) = delete;
  ~EventHolder();

  void registerMouseClickHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseDownHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseUpHandler(EventHandler<MouseEvent> &&handler);
  void registerMouseMoveHandler(EventHandler<MouseEvent> &&handler);

private:
  EventManager *mManager{nullptr};

  std::vector<usize> mMouseClickHandles;
  std::vector<usize> mMouseDownHandles;
  std::vector<usize> mMouseUpHandles;
  std::vector<usize> mMouseMoveHandles;
};

class EventManager {
  friend class EventHolder;
  using MouseEventSet = quoll::SparseSet<EventHandler<MouseEvent>>;

public:
  inline MouseEventSet &getMouseClickHandlers() { return mMouseClickHandlers; }
  inline MouseEventSet &getMouseDownHandlers() { return mMouseDownHandlers; }
  inline MouseEventSet &getMouseUpHandlers() { return mMouseUpHandlers; }
  inline MouseEventSet &getMouseMoveHandlers() { return mMouseMoveHandlers; }
  inline EventHolder createEventHolder() { return EventHolder(this); }

private:
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseClickHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseDownHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseUpHandlers;
  quoll::SparseSet<EventHandler<MouseEvent>> mMouseMoveHandlers;
};

} // namespace qui
