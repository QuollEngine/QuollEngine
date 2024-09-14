#pragma once

#include "quoll/entity/Entity.h"
#include "LuaHeaders.h"
#include "ScriptGlobals.h"

namespace quoll {

template <class T> using sol_maybe = std::variant<T, sol::nil_t>;

} // namespace quoll
