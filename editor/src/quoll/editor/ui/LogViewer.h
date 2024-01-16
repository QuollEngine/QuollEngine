#pragma once

#include "quoll/editor/core/LogMemoryStorage.h"

namespace quoll::editor {

class LogViewer {
public:
  void render(LogMemoryStorage &userLogs);

private:
  void renderLogContainer(const String &name, LogMemoryStorage &logStorage,
                          usize &logSize, f32 width);

private:
  usize mUserLogSize = 0;
};

} // namespace quoll::editor
