#pragma once

#include "LoggerCore.h"

namespace quoll {

LogTransport createStreamTransport(std::ostream &stream);

} // namespace quoll
