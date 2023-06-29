#include "liquid/core/Base.h"
#include "liquid/platform/tools/FileOpener.h"
#include <windows.h>
#include <shellapi.h>

namespace liquid::platform {

void FileOpener::openFile(const Path &path) {
  ShellExecuteA(NULL, "Open", path.string().c_str(), NULL, NULL, SW_SHOW);
}

} // namespace liquid::platform
