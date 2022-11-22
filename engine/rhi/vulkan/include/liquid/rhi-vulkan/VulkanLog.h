#pragma once

namespace liquid::rhi {

#define LOG_DEBUG_VK_NO_HANDLE(stream) LOG_DEBUG("[VK] " << stream)

#define LOG_DEBUG_VK(stream, handle)                                           \
  LOG_DEBUG_VK_NO_HANDLE(stream << " (PTR: " << handle << ")")

} // namespace liquid::rhi
