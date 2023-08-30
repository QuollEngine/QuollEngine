#pragma once

#include "LoggerCore.h"

namespace quoll {

/**
 * @brief Noop log transport
 *
 * Does nothing with incoming log data
 */
constexpr auto NoopLogTransport = [](LogSeverity, LogTimestamp, String) {
  // Do nothing
};

} // namespace quoll
