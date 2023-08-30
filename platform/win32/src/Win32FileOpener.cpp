#include "quoll/core/Base.h"
#include "quoll/platform/tools/FileOpener.h"
#include <windows.h>
#include <shellapi.h>

namespace quoll::platform {

void FileOpener::openFile(const Path &path) {
  ShellExecuteA(NULL, "Open", path.string().c_str(), NULL, NULL, SW_SHOW);
}

} // namespace quoll::platform
