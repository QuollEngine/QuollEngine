#pragma once

namespace quoll::platform {

class FileDialog {
public:
  struct FileTypeEntry {
    quoll::StringView label;

    std::vector<quoll::String> extensions;
  };

public:
  static quoll::Path
  getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes);

  static quoll::Path
  getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes);
};

} // namespace quoll::platform
