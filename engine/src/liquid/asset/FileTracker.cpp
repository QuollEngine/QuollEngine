#include "liquid/core/Base.h"
#include "FileTracker.h"

namespace liquid {

FileTracker::FileTracker(Path path) : mPath(path) {}

std::vector<ChangedFile> FileTracker::trackForChanges() {
  std::vector<ChangedFile> changes;

  std::unordered_map<String, bool> fileVisited;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".meta") {
      continue;
    }
    auto lastWriteTime = std::filesystem::last_write_time(entry);
    auto entryStr = entry.path().string();

    auto foundFile = mFiles.find(entryStr);

    if (foundFile == mFiles.end()) {
      changes.push_back({entry.path(), FileStatus::Created});
    } else if (foundFile->second != lastWriteTime) {
      changes.push_back({entry.path(), FileStatus::Updated});
    }

    mFiles.insert_or_assign(entryStr, lastWriteTime);
    fileVisited.insert_or_assign(entryStr, true);
  }

  for (auto &[file, _] : mFiles) {
    if (fileVisited.find(file) == fileVisited.end()) {
      changes.push_back({Path(file), FileStatus::Deleted});
    }
  }

  return changes;
}

const std::unordered_map<String, std::filesystem::file_time_type> &
FileTracker::getAllTrackedFiles() {
  return mFiles;
}

} // namespace liquid
