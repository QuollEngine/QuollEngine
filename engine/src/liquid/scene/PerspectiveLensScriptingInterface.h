#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

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
class PerspectiveLensScriptingInterface::LuaInterface
    : public ComponentLuaInterface<
          PerspectiveLensScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get near
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getNear(void *state);

  /**
   * @brief Set near
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setNear(void *state);

  /**
   * @brief Get far
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getFar(void *state);

  /**
   * @brief Set far
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setFar(void *state);

  /**
   * @brief Get sensor size
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getSensorSize(void *state);

  /**
   * @brief Set sensor size
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setSensorSize(void *state);

  /**
   * @brief Get focal length
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getFocalLength(void *state);

  /**
   * @brief Set focal length
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setFocalLength(void *state);

  /**
   * @brief Get aperture
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getAperture(void *state);

  /**
   * @brief Set aperture
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setAperture(void *state);

  /**
   * @brief Get shutter speed
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getShutterSpeed(void *state);

  /**
   * @brief Set shutter speed
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setShutterSpeed(void *state);

  /**
   * @brief Get sensitivity
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getSensitivity(void *state);

  /**
   * @brief Set sensitivity
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setSensitivity(void *state);

  /**
   * @brief Delete component
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int deleteThis(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 15> Fields{
      InterfaceField{"get_near", getNear},
      InterfaceField{"set_near", setNear},
      InterfaceField{"get_far", getFar},
      InterfaceField{"set_far", setFar},
      InterfaceField{"get_sensor_size", getSensorSize},
      InterfaceField{"set_sensor_size", setSensorSize},
      InterfaceField{"get_focal_length", getFocalLength},
      InterfaceField{"set_focal_length", setFocalLength},
      InterfaceField{"get_aperture", getAperture},
      InterfaceField{"set_aperture", setAperture},
      InterfaceField{"get_shutter_speed", getShutterSpeed},
      InterfaceField{"set_shutter_speed", setShutterSpeed},
      InterfaceField{"get_sensitivity", getSensitivity},
      InterfaceField{"set_sensitivity", setSensitivity},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "perspective_lens"; }
};

} // namespace quoll
