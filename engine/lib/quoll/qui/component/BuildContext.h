#pragma once

#include "GlobalEventDispatcher.h"

namespace qui {

struct BuildContext {
  GlobalEventDispatcher *globalEvents = nullptr;
};

} // namespace qui
