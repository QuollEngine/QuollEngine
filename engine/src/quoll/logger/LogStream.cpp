#include "quoll/core/Base.h"
#include "LogStream.h"

namespace quoll {

LogStream::LogStream(LogSeverity severity, LogTimestamp timestamp,
                     const LogTransport transport)
    : mSeverity(severity), mTimestamp(timestamp), mTransport(transport) {}

LogStream::LogStream(LogStream &&rhs)
    : mSeverity(rhs.mSeverity), mTimestamp(rhs.mTimestamp),
      mTransport(rhs.mTransport), mStream(std::move(rhs.mStream)) {}

LogStream::~LogStream() {
  if (mStream.tellp() > std::streampos(0)) {
    mTransport(mSeverity, mTimestamp, mStream.str());
  }
}

} // namespace quoll
