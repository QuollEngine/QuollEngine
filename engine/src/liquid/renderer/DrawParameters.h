#pragma once

namespace liquid {

/**
 * @brief Bindless draw parameters
 *
 * Typically passed through push constants
 */
struct DrawParameters {
  /**
   * Index 0
   */
  uint32_t index0 = 0;

  /**
   * Index 1
   */
  uint32_t index1 = 0;

  /**
   * Index 2
   */
  uint32_t index2 = 0;

  /**
   * Index 3
   */
  uint32_t index3 = 0;

  /**
   * Index 4
   */
  uint32_t index4 = 0;

  /**
   * Index 5
   */
  uint32_t index5 = 0;

  /**
   * Index 6
   */
  uint32_t index6 = 0;

  /**
   * Index 7
   */
  uint32_t index7 = 0;

  /**
   * Index 8
   */
  uint32_t index8 = 0;

  /**
   * Index 9
   */
  uint32_t index9 = 0;

  /**
   * Index 10
   */
  uint32_t index10 = 0;

  /**
   * Index 11
   */
  uint32_t index11 = 0;
};

} // namespace liquid
