#pragma once

#include "ReactiveNode.h"

namespace qui::rgraph {

template <std::invocable TFunction>
class ComputationNode
    : public ReactiveNodeWithData<std::invoke_result_t<TFunction>> {
public:
  using Data = std::invoke_result_t<TFunction>;

public:
  constexpr ComputationNode(ReactiveNodeGlobals *globals, TFunction &&func)
      : mGlobals(globals), mFunction(std::forward<TFunction>(func)) {
    mGlobals->activeNode = this;
    mValue = mFunction();
    mGlobals->activeNode = nullptr;
  }

  constexpr Data &get() override {
    if (mGlobals->activeNode) {
      this->addDependent(mGlobals->activeNode);
    }

    return mValue;
  }

  void update() override {
    mGlobals->activeNode = this;
    auto value = mFunction();
    mGlobals->activeNode = nullptr;

    if (mValue != value) {
      mValue = value;
      this->notify();
    }
  }

private:
  ReactiveNodeGlobals *mGlobals;

  Data mValue{};
  TFunction mFunction;
};

} // namespace qui::rgraph

namespace qui {

template <std::invocable TFunction> class Computation {
public:
  constexpr Computation(rgraph::ReactiveNodeGlobals *globals,
                        TFunction &&func) {
    mNode = globals->arena.allocate<rgraph::ComputationNode<TFunction>>(
        globals, std::forward<TFunction>(func));
  }

  constexpr const auto &operator()() const { return mNode->get(); }

  template <std::invocable TObserverFn>
  constexpr auto observe(TObserverFn &&observer) {
    return mNode->observe(std::forward<TObserverFn>(observer));
  }

  constexpr auto *getNode() { return mNode; }

private:
  rgraph::ComputationNode<TFunction> *mNode;
};

} // namespace qui

template <std::invocable TFunction>
struct std::formatter<qui::Computation<TFunction>> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const auto &computation, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "{}", computation());
  }
};
