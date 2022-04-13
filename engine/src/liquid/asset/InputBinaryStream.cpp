#include "liquid/core/Base.h"
#include "InputBinaryStream.h"

namespace liquid {

InputBinaryStream::InputBinaryStream(const std::filesystem::path &path)
    : mStream(path, std::ios::binary | std::ios::in) {}

InputBinaryStream::~InputBinaryStream() { mStream.close(); }

} // namespace liquid
