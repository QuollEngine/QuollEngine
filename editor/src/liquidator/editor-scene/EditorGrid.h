#pragma once

#include "liquid/rhi/ResourceRegistry.h"

namespace liquidator {

/**
 * @brief Editor grid data
 *
 * Passed to hardware buffers
 */
struct EditorGridData {
  /**
   * Grid lines information
   *
   * First value toggles grid lines
   * Second value toggles axis lines
   */
  glm::uvec4 gridLines{1, 1, 0, 0};
};

/**
 * @brief Editor grid
 *
 * Controls editor grid
 */
class EditorGrid {
public:
  /**
   * @brief Set grid lines display flag
   *
   * @param flag Grid lines display flag
   */
  void setGridLinesFlag(bool flag);

  /**
   * @brief Set axis lines display flag
   *
   * @param flag Axis lines display flag
   */
  void setAxisLinesFlag(bool flag);

  /**
   * @brief Check if grid lines are shown
   *
   * @retval true Grid lines are shown
   * @retval false Grid lines are not shown
   */
  inline bool gridLinesShown() const { return mData.gridLines.x == 1; }

  /**
   * @brief Check if axis lines are shown
   *
   * @retval true Axis lines are shown
   * @retval false Axis lines are not shown
   */
  inline bool axisLinesShown() const { return mData.gridLines.y == 1; }

  /**
   * @brief Get editor grid data
   *
   * @return Editor grid data
   */
  inline const EditorGridData &getData() const { return mData; }

private:
  EditorGridData mData;
};

} // namespace liquidator
