#include "quoll/core/Base.h"
#include "Math.h"

namespace quoll {

void LuaMath::create(sol::state_view state) {
  auto vec3 = state.new_usertype<glm::vec3>("Vector3");

  vec3["x"] = &glm::vec3::x;
  vec3["y"] = &glm::vec3::y;
  vec3["z"] = &glm::vec3::z;
}

} // namespace quoll
