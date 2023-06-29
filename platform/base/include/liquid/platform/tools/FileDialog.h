#pragma once

namespace liquid::platform {

/**
 * @brief Platform file dialog
 */
class FileDialog {
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
  static liquid::Path
  getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes);

  /**
   * @brief Get file path from OS create file dialog
   *
   * @param fileTypes Supported file types
   * @return Chosen or empty file path
   */
  static liquid::Path
  getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes);
};

} // namespace liquid::platform
