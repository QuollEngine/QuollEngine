#include "quoll/core/Base.h"
#include "quoll/asset/FileTracker.h"

#include "quoll-tests/Testing.h"

namespace fs = std::filesystem;

static const fs::path fileTrackerPath = FixturesPath / "file-tracker-test";

bool compareChangedFiles(quoll::ChangedFile a, quoll::ChangedFile b) {
  return a.path.string() < b.path.string();
}

class FileTrackerTest : public ::testing::Test {
public:
  FileTrackerTest() : fileTracker(fileTrackerPath) {}

  void createFixtures(usize count) {
    for (usize i = 0; i < count; ++i) {
      {
        fs::path path(fileTrackerPath / ("file-" + std::to_string(i)));
        std::ofstream stream(path);
        stream.close();
      }

      {
        fs::path innerPath(fileTrackerPath / "inner-dir" /
                           ("file-" + std::to_string(i)));
        std::ofstream stream(innerPath);
        stream.close();
      }
    }
  }

  quoll::FileTracker fileTracker;

protected:
  void SetUp() override {
    fs::create_directory(fileTrackerPath);
    fs::create_directory(fileTrackerPath / "inner-dir");
  }

  void TearDown() override { fs::remove_all(fileTrackerPath); }
};

TEST_F(FileTrackerTest, ReturnsEmptyMapIfNoTracking) {
  const auto &files = fileTracker.getAllTrackedFiles();
  EXPECT_EQ(files.size(), 0);
}

TEST_F(FileTrackerTest, ReturnsEmptyMapIfNoFilesAreTracked) {
  const auto &files = fileTracker.getAllTrackedFiles();
  EXPECT_EQ(files.size(), 0);
}

TEST_F(FileTrackerTest, ReturnsEmptyMapIfTrackingIsEmpty) {
  fileTracker.trackForChanges();
  const auto &files = fileTracker.getAllTrackedFiles();
  EXPECT_EQ(files.size(), 0);
}

TEST_F(FileTrackerTest, FirstTrackTreatsAllFilesAsCreated) {
  createFixtures(2);

  auto files = fileTracker.trackForChanges();
  EXPECT_EQ(files.size(), 4);

  std::sort(files.begin(), files.end(), compareChangedFiles);

  {
    fs::path path(fileTrackerPath / "file-0");
    EXPECT_EQ(files.at(0).path, path);
    EXPECT_EQ(files.at(0).status, quoll::FileStatus::Created);
  }

  {
    fs::path path(fileTrackerPath / "file-1");
    EXPECT_EQ(files.at(1).path, path);
    EXPECT_EQ(files.at(1).status, quoll::FileStatus::Created);
  }

  {
    fs::path path(fileTrackerPath / "inner-dir" / "file-0");
    EXPECT_EQ(files.at(2).path, path);
    EXPECT_EQ(files.at(2).status, quoll::FileStatus::Created);
  }

  {
    fs::path path(fileTrackerPath / "inner-dir" / "file-1");
    EXPECT_EQ(files.at(3).path, path);
    EXPECT_EQ(files.at(3).status, quoll::FileStatus::Created);
  }
}

TEST_F(FileTrackerTest, TracksUpdatedFile) {
  createFixtures(2);
  fileTracker.trackForChanges();

  fs::path changedFilePath(fileTrackerPath / "file-0");

  auto oldTime = fs::last_write_time(changedFilePath);
  fs::last_write_time(changedFilePath, oldTime + std::chrono::hours(24));

  const auto &files = fileTracker.trackForChanges();
  EXPECT_EQ(files.size(), 1);
  EXPECT_EQ(files.at(0).status, quoll::FileStatus::Updated);
  EXPECT_EQ(files.at(0).path, changedFilePath);
}

TEST_F(FileTrackerTest, TracksCreatedFile) {
  createFixtures(2);
  fileTracker.trackForChanges();

  fs::path changedFilePath(fileTrackerPath / "file-20");
  {
    std::ofstream stream(changedFilePath);
    stream.close();
  }

  const auto &files = fileTracker.trackForChanges();
  EXPECT_EQ(files.size(), 1);
  EXPECT_EQ(files.at(0).status, quoll::FileStatus::Created);
  EXPECT_EQ(files.at(0).path, changedFilePath);
}

TEST_F(FileTrackerTest, TracksDeletedFile) {
  createFixtures(2);
  fileTracker.trackForChanges();

  fs::path changedFilePath(fileTrackerPath / "file-0");
  fs::remove(changedFilePath);

  const auto &files = fileTracker.trackForChanges();
  EXPECT_EQ(files.size(), 1);
  EXPECT_EQ(files.at(0).status, quoll::FileStatus::Deleted);
  EXPECT_EQ(files.at(0).path, changedFilePath);
}

TEST_F(FileTrackerTest, TracksChanges) {
  createFixtures(2);
  fileTracker.trackForChanges();

  fs::path changedFilePath1(fileTrackerPath / "file-0");
  auto oldTime = fs::last_write_time(changedFilePath1);
  fs::last_write_time(changedFilePath1, oldTime + std::chrono::hours(24));

  fs::path changedFilePath2(fileTrackerPath / "file-1");
  fs::remove(changedFilePath2);

  fs::path changedFilePath3(fileTrackerPath / "file-3");
  {
    std::ofstream stream(changedFilePath3);
    stream.close();
  }

  auto files = fileTracker.trackForChanges();
  EXPECT_EQ(files.size(), 3);

  std::sort(files.begin(), files.end(), compareChangedFiles);

  EXPECT_EQ(files.at(0).status, quoll::FileStatus::Updated);
  EXPECT_EQ(files.at(0).path, changedFilePath1);
  EXPECT_EQ(files.at(1).status, quoll::FileStatus::Deleted);
  EXPECT_EQ(files.at(1).path, changedFilePath2);
  EXPECT_EQ(files.at(2).status, quoll::FileStatus::Created);
  EXPECT_EQ(files.at(2).path, changedFilePath3);
}
