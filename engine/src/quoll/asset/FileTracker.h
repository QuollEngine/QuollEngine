#pragma once

namespace quoll {

enum class FileStatus { Created, Updated, Deleted };

/**
 * @brief Chagned file object
 */
struct ChangedFile {
  /**
   * Path to changed file
   */
  Path path;

  /**
   * Change status
   */
  FileStatus status;
};

/**
 * @brief File tracker
 *
 * Tracks files that were changed
 * since last track
 */
class FileTracker {
  using TrackedFileMap =
      std::unordered_map<String, std::filesystem::file_time_type>;

public:
  /**
   * @brief Create file tracker
   *
   * @param path Path to track
   */
  FileTracker(Path path);

  /**
   * @brief Track for changes
   *
   * @return Changed files
   */
  std::vector<ChangedFile> trackForChanges();

  /**
   * @brief Get all tracked files
   *
   * @return All tracked files
   */
  const TrackedFileMap &getAllTrackedFiles();

private:
  TrackedFileMap mFiles;
  Path mPath;
};

} // namespace quoll
