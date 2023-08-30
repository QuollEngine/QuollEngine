#pragma once

#include "LoggerCore.h"

namespace quoll {

/**
 * @brief Create stream transport
 *
 * @param stream Output stream
 * @return Log transport
 */
LogTransport createStreamTransport(std::ostream &stream);

} // namespace quoll
