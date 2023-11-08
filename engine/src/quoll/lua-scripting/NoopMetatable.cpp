#include "quoll/core/Base.h"

#include "NoopMetatable.h"

namespace quoll::lua {

NoopMetatable NoopMetatable::call() { return {}; }

NoopMetatable NoopMetatable::index() { return {}; }

void NoopMetatable::create(sol::state &state) {
  state.new_usertype<NoopMetatable>(
      "Noop",
      // Call
      sol::meta_function::call_function, &NoopMetatable::call,

      // Index
      sol::meta_function::index, &NoopMetatable::index);
}

} // namespace quoll::lua
