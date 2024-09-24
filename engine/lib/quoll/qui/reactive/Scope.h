#pragma once

#include "Computation.h"
#include "Signal.h"

namespace qui {

class Scope {
public:
  template <typename TData> auto signal(const TData &data) {
    return Signal(&mGlobals, data);
  }

  template <typename TData> auto signal(TData &&data) {
    return Signal(&mGlobals, std::forward<TData>(data));
  }

  template <typename TData> auto signal(std::initializer_list<TData> &&data) {
    return signal(std::vector(data));
  }

  auto signal(const char *data) { return signal<quoll::String>(data); }

  template <std::invocable TFunction> auto computation(TFunction &&fn) {
    return Computation(&mGlobals, std::forward<TFunction>(fn));
  }

  template <typename... TArgs>
  auto format(std::format_string<TArgs...> fmt, TArgs &&...args) {
    return computation([fmt, ... args = args]() mutable {
      return std::format(fmt, std::forward<TArgs>(args)...);
    });
  }

private:
  rgraph::ReactiveNodeGlobals mGlobals{};
};

} // namespace qui
