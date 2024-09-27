#pragma once

#include "../component/Element.h"
#include "Computation.h"
#include "Signal.h"

namespace qui {

template <typename TData> class StaticData {
public:
  constexpr StaticData() = default;
  constexpr StaticData(TData &&data) : mData(std::forward<TData>(data)) {}
  constexpr StaticData(const TData &data) : mData(data) {}

  constexpr const TData &get() const { return mData; }
  constexpr TData &get() { return mData; }

private:
  TData mData;
};

template <typename TData> class Value {
  using ReactiveValue = rgraph::ReactiveNodeWithData<TData>;
  using StaticValue = StaticData<TData>;

public:
  constexpr Value() : mData(StaticValue()) {}
  constexpr Value(TData &&data)
      : mData(StaticValue(std::forward<TData>(data))) {}
  constexpr Value(const TData &data) : mData(StaticValue(data)) {}

  constexpr Value(const char *data)
  requires std::same_as<TData, quoll::String>
      : mData(quoll::String(data)) {}

  template <std::derived_from<Component> TComp>
  requires std::same_as<TData, Element>
  constexpr Value(TComp comp) : mData(Element(comp)) {}

  constexpr Value(std::initializer_list<Element> elements)
  requires std::same_as<TData, std::vector<Element>>
      : mData(std::vector<Element>{elements}) {}

  template <std::derived_from<Component> TComp>
  requires std::same_as<TData, std::vector<Element>>
  constexpr Value(TComp comp) : mData(std::vector<Element>{comp}) {}

  constexpr Value(Signal<TData> signal) : mData(signal.getNode()) {}

  template <std::invocable TFunction>
  constexpr Value(Computation<TFunction> computation)
      : mData(computation.getNode()) {}

  constexpr Value(const Value &) = default;
  constexpr Value(Value &&) = default;

  constexpr Value &operator=(const Value &rhs) {
    for (auto &observer : mObservers) {
      observer.unobserve();
    }
    mObservers.clear();

    mData = rhs.mData;
    return *this;
  }

  constexpr Value &operator=(Value &&rhs) {
    for (auto &observer : mObservers) {
      observer.unobserve();
    }
    mObservers.clear();

    mData = std::move(rhs.mData);
    return *this;
  }

  constexpr const TData &operator()() const {
    if (auto *staticData = std::get_if<StaticValue>(&mData)) {
      return staticData->get();
    }

    return std::get<ReactiveValue *>(mData)->get();
  }

  constexpr TData &operator()() {
    if (auto *staticData = std::get_if<StaticValue>(&mData)) {
      return staticData->get();
    }

    return std::get<ReactiveValue *>(mData)->get();
  }

  template <std::invocable TFunction> constexpr void observe(TFunction &&func) {
    if (auto *node = std::get_if<ReactiveValue *>(&mData)) {
      auto observer = (*node)->observe(std::forward<TFunction>(func));
      mObservers.push_back(observer);
    }
  }

private:
  std::variant<ReactiveValue *, StaticValue> mData;
  std::vector<rgraph::ReactiveNodeObserver> mObservers;
};

} // namespace qui
