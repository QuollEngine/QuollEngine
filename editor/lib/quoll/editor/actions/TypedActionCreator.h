#pragma once

#include "ActionCreator.h"

namespace quoll::editor {

template <class TAction, class... TArgs>
class TypedActionCreatorImpl : public ActionCreator {
public:
  TypedActionCreatorImpl(TArgs &&...args) : mArgs(args...) {}

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

class TypedActionCreator {
public:
  template <class TAction, class... TArgs>
  static TypedActionCreatorImpl<TAction, TArgs...> *create(TArgs &&...args) {
    return new TypedActionCreatorImpl<TAction, TArgs...>(
        std::forward<TArgs>(args)...);
  }
};

} // namespace quoll::editor
