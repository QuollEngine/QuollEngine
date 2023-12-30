#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua interface for perspective lens
 */
class PerspectiveLensLuaTable {
public:
  /**
   * @brief Create perspective lens table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  PerspectiveLensLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get near plane value
   *
   * @return Near plane value
   */
  sol_maybe<f32> getNear();

  /**
   * @brief Set near plane value
   *
   * @param near Near plane value
   */
  void setNear(f32 near);

  /**
   * @brief Get far plane value
   *
   * @return Far plane value
   */
  sol_maybe<f32> getFar();

  /**
   * @brief Set far plane value
   *
   * @param far Far plane vavoid *statelue
   */
  void setFar(f32 far);

  /**
   * @brief Get sensor size
   *
   * @return Sensor width and height
   */
  std::tuple<sol_maybe<f32>, sol_maybe<f32>> getSensorSize();

  /**
   * @brief Set sensor size
   *
   * @param width Sensor width
   * @param height Sensor height
   */
  void setSensorSize(f32 width, f32 height);

  /**
   * @brief Get near plane value
   *
   * @return Focal length
   */
  sol_maybe<f32> getFocalLength();

  /**
   * @brief Set focal length
   *
   * @param focalLength Focal length
   */
  void setFocalLength(f32 focalLength);

  /**
   * @brief Get aperture
   *
   * @return Aperture
   */
  sol_maybe<f32> getAperture();

  /**
   * @brief Set aperture
   *
   * @param aperture Aperture
   */
  void setAperture(f32 aperture);

  /**
   * @brief Get shutter speed
   *
   * @return Shutter speed
   */
  sol_maybe<f32> getShutterSpeed();

  /**
   * @brief Set shutter speed
   *
   * @param shutterSpeed Shutter speed
   */
  void setShutterSpeed(f32 shutterSpeed);

  /**
   * @brief Get sensitivity
   *
   * @return Sensitivity
   */
  sol_maybe<u32> getSensitivity();

  /**
   * @brief Set sensitivity
   *
   * @param sensitivity Sensitivity
   */
  void setSensitivity(u32 sensitivity);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void create(sol::usertype<PerspectiveLensLuaTable> usertype);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "perspectiveLens"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
