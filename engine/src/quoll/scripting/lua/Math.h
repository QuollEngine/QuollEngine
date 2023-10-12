#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Math interfaces
 */
class LuaMath {
public:
  /**
   * @brief Create interfaces
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);
};

} // namespace quoll
