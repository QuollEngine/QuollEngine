#pragma once

#include "Action.h"

namespace quoll::editor {

class ActionCreator {
public:
  virtual ~ActionCreator() = default;

  virtual std::unique_ptr<Action> create() = 0;
};

} // namespace quoll::editor
