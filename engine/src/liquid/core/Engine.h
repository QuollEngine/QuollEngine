#pragma once

namespace liquid {

/**
 * @brief Global engine singleton
 *
 * This singleton is used to store only
 * global read only variables that are set only
 * once by the application and read from multiple
 * modules.
 */
class Engine {
private:
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static Engine engine;

public:
  /**
   * @brief Set path for engine data
   *
   * @param path Engine path
   */
  static void setPath(const Path &path);

  /**
   * @brief Get path for engine shaders
   *
   * @return Engine shaders path
   */
  static const Path getShadersPath();

  /**
   * @brief Get path for engine fonts
   *
   * @return Engine fonts path
   */
  static const Path getFontsPath();

  /**
   * @brief Get engine path
   *
   * @return Engine path
   */
  static const Path getEnginePath();

private:
  /**
   * @brief Create engine
   *
   * This constructor is private in order to disallow
   * creating individual engine object
   */
  Engine() = default;

private:
  Path mAssetsPath;
  Path mEnginePath;
};

} // namespace liquid
