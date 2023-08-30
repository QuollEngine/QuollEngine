#include "liquid/core/Base.h"
#include "OutputBinaryStream.h"

namespace quoll {

OutputBinaryStream::OutputBinaryStream(Path path)
    : mStream(path, std::ios::binary | std::ios::out) {}

OutputBinaryStream::~OutputBinaryStream() { mStream.close(); }

} // namespace quoll
