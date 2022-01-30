#pragma once

#include "liquid/core/Base.h"

namespace liquid::platform_tools {

class NativeFileDialog {
public:
  /**
   * @brief Get file path from OS file dialog
   *
   * @param extensions File extensions to show
   * @return Chosen file path or empty string if cancelled
   */
  liquid::String
  getFilePathFromDialog(const std::vector<liquid::String> &extensions);
};

} // namespace liquid::platform_tools
