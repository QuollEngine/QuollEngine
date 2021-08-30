#pragma once

#include "core/Base.h"

class NativeFileDialog {
public:
  liquid::String
  getFilePathFromDialog(const std::vector<liquid::String> &extensions);
  liquid::String getDirectoryPathFromDialog();
};
