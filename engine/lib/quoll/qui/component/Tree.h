#pragma once

#include "Element.h"
#include "EventContext.h"
#include "GlobalEventDispatcher.h"

namespace qui {

struct Tree {
  EventContext eventContext;

  std::unique_ptr<GlobalEventDispatcher> globalEvents{
      new GlobalEventDispatcher};
  Element root;
};

} // namespace qui
