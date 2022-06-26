#include "liquid/core/Base.h"
#include "NativeFileDialog.h"

namespace liquid::platform_tools {

liquid::Path NativeFileDialog::getFilePathFromDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  LIQUID_ASSERT(false, "Not implemented");
  return "";
}

liquid::Path NativeFileDialog::getFilePathFromCreateDialog(
    const std::vector<FileTypeEntry> &fileTypes) {
  LIQUID_ASSERT(false, "Not implemented");
  return "";
}

} // namespace liquid::platform_tools
