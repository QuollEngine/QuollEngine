#pragma once

namespace quoll::platform {

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
    quoll::StringView label;

    /**
     * Entry extensions
     */
    std::vector<quoll::String> extensions;
  };

public:
  /**
   * @brief Get file path from OS file dialog
   *
   * @param fileTypes Supported file types
   * @return Chosen or empty file path
   */
  static quoll::Path
  getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes);

  /**
   * @brief Get file path from OS create file dialog
   *
   * @param fileTypes Supported file types
   * @return Chosen or empty file path
   */
  static quoll::Path
  getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes);
};

} // namespace quoll::platform
