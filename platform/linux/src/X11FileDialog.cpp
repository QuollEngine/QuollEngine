#include "quoll/core/Base.h"
#include "quoll/platform/tools/FileDialog.h"

namespace quoll::platform {

Path FileDialog::getFilePathFromDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  LIQUID_ASSERT(false, "Not implemented");
  return "";
}

Path FileDialog::getFilePathFromCreateDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  LIQUID_ASSERT(false, "Not implemented");
  return "";
}

} // namespace quoll::platform
