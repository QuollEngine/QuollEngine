#include "liquid/core/Base.h"
#include "platform-tools/NativeFileDialog.h"

namespace liquid::platform_tools {

liquid::String NativeFileDialog::getFilePathFromDialog(
    const std::vector<liquid::String> &extensions) {
  LIQUID_ASSERT(false, "Not implemented");
  return "";
}

} // namespace liquid::platform_tools
