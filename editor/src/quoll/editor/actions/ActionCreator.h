#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Action creator
 */
class ActionCreator {
public:
  /**
   * @brief Default destructor
   */
  virtual ~ActionCreator() = default;

  /**
   * @brief Create action
   *
   * @return Action
   */
  virtual std::unique_ptr<Action> create() = 0;
};

} // namespace quoll::editor
