#include "liquid/core/Base.h"
#include "liquid/platform/tools/FileDialog.h"

namespace liquid::platform {

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

} // namespace liquid::platform