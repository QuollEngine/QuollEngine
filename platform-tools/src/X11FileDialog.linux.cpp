#include "liquid/core/Base.h"
#include "platform-tools/NativeFileDialog.h"

namespace liquid::platform_tools {

liquid::String NativeFileDialog::getFilePathFromDialog(
    const std::vector<liquid::String> &extensions) {
  throw std::runtime_error("Not implemented");
}

} // namespace liquid::platform_tools
