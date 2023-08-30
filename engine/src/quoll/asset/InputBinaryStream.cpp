#include "quoll/core/Base.h"
#include "InputBinaryStream.h"

namespace quoll {

InputBinaryStream::InputBinaryStream(const Path &path)
    : mStream(path, std::ios::binary | std::ios::in) {}

InputBinaryStream::~InputBinaryStream() { mStream.close(); }

} // namespace quoll
