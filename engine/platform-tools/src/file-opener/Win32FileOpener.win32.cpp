#include "liquid/core/Base.h"
#include "NativeFileOpener.h"
#include <windows.h>
#include <shellapi.h>

namespace liquid::platform_tools {

void NativeFileOpener::openFile(const Path &path) {
  ShellExecuteA(NULL, "Open", path.string().c_str(), NULL, NULL, SW_SHOW);
}

} // namespace liquid::platform_tools
