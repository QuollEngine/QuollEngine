#include "quoll/core/Base.h"
#include "MathLuaTable.h"

namespace quoll {

void MathLuaTable::create(sol::state_view state) {
  // Vector3
  auto vec3 = state.new_usertype<glm::vec3>("Vector3");

  vec3[sol::meta_function::construct] =
      sol::factories([](f32 x, f32 y, f32 z) { return glm::vec3(x, y, z); });

  vec3["x"] = &glm::vec3::x;
  vec3["y"] = &glm::vec3::y;
  vec3["z"] = &glm::vec3::z;

  // Quaternion
  auto quat = state.new_usertype<glm::quat>("Quaternion");

  quat[sol::meta_function::construct] = sol::factories(
      [](f32 x, f32 y, f32 z, f32 w) { return glm::quat(w, x, y, z); });
  quat["fromEulerAngles"] = sol::factories([](f32 x, f32 y, f32 z) {
    return glm::toQuat(glm::eulerAngleXYZ(x, y, z));
  });
  quat["toEulerAngles"] = sol::factories([](glm::quat q) {
    glm::vec3 euler{};
    glm::extractEulerAngleXYZ(glm::toMat4(q), euler.x, euler.y, euler.z);
    return glm::vec3{euler.x, euler.y, euler.z};
  });

  quat["x"] = &glm::quat::x;
  quat["y"] = &glm::quat::y;
  quat["z"] = &glm::quat::z;
  quat["w"] = &glm::quat::w;
}

} // namespace quoll
