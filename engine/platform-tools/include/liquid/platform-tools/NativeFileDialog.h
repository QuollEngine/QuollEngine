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
   * @brief File type entry
   */
  struct FileTypeEntry {
    /**
     * Entry label
     */
    liquid::StringView label;

    /**
     * Entry extensions
     */
    std::vector<liquid::String> extensions;
  };

public:
  /**
   * @brief Get file path from OS file dialog
   *
   * @param fileTypes Supported file types
   * @return Chosen or empty file path
   */
  liquid::Path
  getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes);

  /**
   * @brief Get file path from OS create file dialog
   *
   * @param fileTypes Supported file types
   * @return Chosen or empty file path
   */
  liquid::Path
  getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes);
};

} // namespace liquid::platform_tools
