#pragma once

#include "../common/Traits.h"
#include "ReactiveNode.h"

namespace qui::rgraph {

template <typename TData>
class SignalNode : public ReactiveNodeWithData<TData> {
public:
  using Data = TData;

public:
  constexpr SignalNode(ReactiveNodeGlobals *globals) : mGlobals(globals) {}
  constexpr SignalNode(ReactiveNodeGlobals *globals, const Data &value)
      : mGlobals(globals), mValue(value) {}
  constexpr SignalNode(ReactiveNodeGlobals *globals, Data &&value)
      : mGlobals(globals), mValue(std::forward<Data>(value)) {}

  constexpr Data &get() override {
    if (mGlobals->activeNode) {
      this->addDependent(mGlobals->activeNode);
    }

    return mValue;
  }

  constexpr void set(Data &&value) {
    if (mValue == value) {
      return;
    }
    mValue = value;
    this->notify();
  }

  constexpr void set(const Data &value) {
    if (mValue == value) {
      return;
    }
    mValue = value;
    this->notify();
  }

  void update() override {}

private:
  ReactiveNodeGlobals *mGlobals;

  Data mValue{};
};

} // namespace qui::rgraph

namespace qui {

template <typename TData> class Signal {
public:
  constexpr Signal(rgraph::ReactiveNodeGlobals *globals, TData &&data) {
    mNode = globals->arena.allocate<rgraph::SignalNode<TData>>(
        globals, std::forward<TData>(data));
  }

  constexpr Signal(rgraph::ReactiveNodeGlobals *globals, const TData &data) {
    mNode = globals->arena.allocate<rgraph::SignalNode<TData>>(globals, data);
  }

  constexpr const auto &operator()() const { return mNode->get(); }

  constexpr void set(TData &&data) { mNode->set(std::forward<TData>(data)); }

  constexpr void set(const TData &data) { mNode->set(data); }

  template <std::invocable ObserverFn>
  constexpr auto observe(ObserverFn &&observer) {
    return mNode->observe(std::forward<ObserverFn>(observer));
  }

  template <typename TVector = TData>
  void push_back(typename TVector::value_type &&value)
  requires concepts::is_vector<TData>
  {
    mNode->get().push_back(std::forward<typename TVector::value_type>(value));
    mNode->notify();
  }

  constexpr auto *getNode() { return mNode; }

private:
  rgraph::SignalNode<TData> *mNode;
};

} // namespace qui

template <typename TData> struct std::formatter<qui::Signal<TData>> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const auto &signal, FormatContext &ctx) const {
    return std::format_to(ctx.out(), "{}", signal());
  }
};
