#pragma once

#include "Element.h"
#include "EventManager.h"

namespace qui {

struct Tree {
  std::unique_ptr<EventManager> events{new EventManager};
  Element root;
};

} // namespace qui
