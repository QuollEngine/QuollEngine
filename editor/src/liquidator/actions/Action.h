#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/ui/FontAwesome.h"

namespace liquid::editor {

/**
 * @brief Action executor result
 */
struct ActionExecutorResult {
  /**
   * Entities to save
   */
  std::vector<Entity> entitiesToSave;

  /**
   * Entities to delete
   */
  std::vector<Entity> entitiesToDelete;

  /**
   * Save scene
   */
  bool saveScene = false;
};

/**
 * @brief Editor action interface
 */
class Action {
public:
  /**
   * @brief Action destructor
   */
  virtual ~Action() = default;

  Action() = default;
  Action(const Action &) = delete;
  Action(Action &&) = delete;
  Action &operator=(const Action &) = delete;
  Action &operator=(Action &&) = delete;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  virtual ActionExecutorResult onExecute(WorkspaceState &state) = 0;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  virtual bool predicate(WorkspaceState &state) = 0;
};

} // namespace liquid::editor
