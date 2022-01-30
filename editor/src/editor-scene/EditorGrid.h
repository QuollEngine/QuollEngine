#pragma once

#include "liquid/renderer/ResourceAllocator.h"
#include "liquid/renderer/HardwareBuffer.h"

namespace liquidator {

class EditorGrid {
  struct EditorGridData {
    glm::uvec4 gridLines{1, 1, 0, 0};
  };

public:
  /**
   * @brief Create editor grid
   *
   * @param resourceAllocator Resource allocator
   */
  EditorGrid(liquid::ResourceAllocator *resourceAllocator);

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
  inline bool gridLinesShown() const { return data.gridLines.x == 1; }

  /**
   * @brief Check if axis lines are shown
   *
   * @retval true Axis lines are shown
   * @retval false Axis lines are not shown
   */
  inline bool axisLinesShown() const { return data.gridLines.y == 1; }

  /**
   * @brief Get uniform buffer
   *
   * @return Uniform buffer
   */
  inline const liquid::SharedPtr<liquid::HardwareBuffer> &
  getUniformBuffer() const {
    return buffer;
  }

private:
  /**
   * @brief Update uniform buffer
   */
  void updateUniformBuffer();

private:
  EditorGridData data;
  liquid::SharedPtr<liquid::HardwareBuffer> buffer;
};

} // namespace liquidator
