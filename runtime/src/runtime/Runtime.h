#pragma once

#include "LaunchConfig.h"

namespace quoll::runtime {

class Runtime {
public:
  Runtime(const LaunchConfig &config);

  void start();

private:
  LaunchConfig mConfig;
};

} // namespace quoll::runtime
