#pragma once

namespace quoll {

enum class FileStatus { Created, Updated, Deleted };

struct ChangedFile {
  Path path;

  FileStatus status;
};

class FileTracker {
  using TrackedFileMap =
      std::unordered_map<String, std::filesystem::file_time_type>;

public:
  FileTracker(Path path);

  std::vector<ChangedFile> trackForChanges();

  const TrackedFileMap &getAllTrackedFiles();

private:
  TrackedFileMap mFiles;
  Path mPath;
};

} // namespace quoll
