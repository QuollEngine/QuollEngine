#pragma once

namespace liquid::platform_tools {

/**
 * @brief Native file dialog
 *
 * Abstracts away platform specific
 * file dialog implementation over
 * a common API
 */
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

  /**
   * @brief Get file path from OS create file dialog
   *
   * @param extensions File extensions to show
   * @return Chosen file path or empty string if cancelled
   */
  std::filesystem::path
  getFilePathFromCreateDialog(const std::vector<liquid::String> &extensions);
};

} // namespace liquid::platform_tools
