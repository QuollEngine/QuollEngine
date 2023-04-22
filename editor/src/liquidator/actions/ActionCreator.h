#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

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

} // namespace liquid::editor
