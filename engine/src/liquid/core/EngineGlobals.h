#pragma once

#include "Base.h"
#include "Logger.h"

namespace liquid {

static Logger engineLogger(Logger::Debug);

#if defined(LIQUID_DEBUG)
#define LOG_DEBUG(stream) engineLogger.log(Logger::Debug) << stream
#else
#define LOG_DEBUG(_)                                                           \
  {}
#endif

} // namespace liquid
