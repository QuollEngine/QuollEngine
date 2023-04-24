#pragma once

#include "ActionCreator.h"

namespace liquid::editor {

/**
 * @brief Typed action creator implementation
 *
 * @tparam TAction Action type
 * @tparam ...TArgs Action parameters type
 */
template <class TAction, class... TArgs>
class TypedActionCreatorImpl : public ActionCreator {
public:
  /**
   * @brief Create action creator
   *
   * @param ...args Action parameters
   */
  TypedActionCreatorImpl(TArgs &&...args) : mArgs(args...) {}

  /**
   * @brief Create action
   *
   * @return Action
   */
  std::unique_ptr<Action> create() override {
    auto args = mArgs;

    // Pass tuple as action constructor arguments
    TAction *action =
        std::apply([](auto &...args) { return new TAction(args...); }, args);

    return std::unique_ptr<Action>(action);
  }

private:
  std::tuple<TArgs...> mArgs;
};

/**
 * @brief Typed action creator
 */
class TypedActionCreator {
public:
  /**
   * @brief Create typed action creator
   *
   * @tparam TAction Action type
   * @tparam ...TArgs Action parameters type
   * @param ...args Action parameters
   * @return Typed action creator
   */
  template <class TAction, class... TArgs>
  static TypedActionCreatorImpl<TAction, TArgs...> *create(TArgs &&...args) {
    return new TypedActionCreatorImpl<TAction, TArgs...>(
        std::forward<TArgs>(args)...);
  }
};

} // namespace liquid::editor
