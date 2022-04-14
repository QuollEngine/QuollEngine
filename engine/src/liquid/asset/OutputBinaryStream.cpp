#include "liquid/core/Base.h"
#include "OutputBinaryStream.h"

namespace liquid {

OutputBinaryStream::OutputBinaryStream(std::filesystem::path path)
    : mStream(path, std::ios::binary | std::ios::out) {}

OutputBinaryStream::~OutputBinaryStream() { mStream.close(); }

} // namespace liquid
