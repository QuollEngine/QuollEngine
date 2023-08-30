#pragma once

#include "quoll/editor/actions/Action.h"

namespace quoll::editor {

/**
 * @brief Action creator
 */
class ActionCreator {
public:
  /**
   * @brief Create action
   *
   * @return Action
   */
  virtual std::unique_ptr<Action> create() = 0;
};

} // namespace quoll::editor
