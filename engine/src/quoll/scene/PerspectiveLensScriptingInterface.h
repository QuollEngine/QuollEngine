#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for perspective lens
 */
struct PerspectiveLensScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua interface for perspective lens
 */
class PerspectiveLensScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create perspective lens table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get near plane value
   *
   * @return Near plane value
   */
  sol_maybe<float> getNear();

  /**
   * @brief Set near plane value
   *
   * @param near Near plane value
   */
  void setNear(float near);

  /**
   * @brief Get far plane value
   *
   * @return Far plane value
   */
  sol_maybe<float> getFar();

  /**
   * @brief Set far plane value
   *
   * @param far Far plane vavoid *statelue
   */
  void setFar(float far);

  /**
   * @brief Get sensor size
   *
   * @return Sensor width and height
   */
  std::tuple<sol_maybe<float>, sol_maybe<float>> getSensorSize();

  /**
   * @brief Set sensor size
   *
   * @param width Sensor width
   * @param height Sensor height
   */
  void setSensorSize(float width, float height);

  /**
   * @brief Get near plane value
   *
   * @return Focal length
   */
  sol_maybe<float> getFocalLength();

  /**
   * @brief Set focal length
   *
   * @param focalLength Focal length
   */
  void setFocalLength(float focalLength);

  /**
   * @brief Get aperture
   *
   * @return Aperture
   */
  sol_maybe<float> getAperture();

  /**
   * @brief Set aperture
   *
   * @param aperture Aperture
   */
  void setAperture(float aperture);

  /**
   * @brief Get shutter speed
   *
   * @return Shutter speed
   */
  sol_maybe<float> getShutterSpeed();

  /**
   * @brief Set shutter speed
   *
   * @param shutterSpeed Shutter speed
   */
  void setShutterSpeed(float shutterSpeed);

  /**
   * @brief Get sensitivity
   *
   * @return Sensitivity
   */
  sol_maybe<uint32_t> getSensitivity();

  /**
   * @brief Set sensitivity
   *
   * @param sensitivity Sensitivity
   */
  void setSensitivity(uint32_t sensitivity);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void create(
      sol::usertype<PerspectiveLensScriptingInterface::LuaInterface> usertype);

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "perspective_lens"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
