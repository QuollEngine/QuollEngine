#pragma once

#include "Element.h"
#include "EventContext.h"
#include "EventManager.h"

namespace qui {

struct Tree {
  EventContext eventContext;

  std::unique_ptr<EventManager> events{new EventManager};
  Element root;
};

} // namespace qui
