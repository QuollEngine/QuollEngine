#pragma once

namespace liquid::rhi {

enum class ShaderHandle : uint32_t { Invalid = 0 };

enum class BufferHandle : uint32_t { Invalid = 0 };

enum class TextureHandle : uint32_t { Invalid = 0 };

enum class TextureViewHandle : uint32_t { Invalid = 0 };

enum class RenderPassHandle : uint32_t { Invalid = 0 };

enum class FramebufferHandle : uint32_t { Invalid = 0 };

enum class PipelineHandle : uint32_t { Invalid = 0 };

enum class DescriptorLayoutHandle : uint32_t { Invalid = 0 };

enum class DescriptorHandle : uint32_t { Invalid = 0 };

/**
 * @brief Check if type equals any of the other types
 *
 * @tparam T Type
 * @tparam ...Rest Other types
 */
template <class T, class... Rest>
inline constexpr bool IsAnySame = (std::is_same_v<T, Rest> || ...);

/**
 * @brief Check if handle is valid
 *
 * @tparam THandle Handle type
 * @param handle Handle
 * @retval true Handle is valid
 * @retval false Handle is not valid
 */
template <class THandle> constexpr inline bool isHandleValid(THandle handle) {
  static_assert(
      IsAnySame<THandle, ShaderHandle, BufferHandle, TextureHandle,
                TextureViewHandle, RenderPassHandle, FramebufferHandle,
                PipelineHandle, DescriptorLayoutHandle, DescriptorHandle>,
      "Type must be a render handle");
  return handle != THandle::Invalid;
}

/**
 * @brief Cast handle to uint
 *
 * @tparam THandle Handle type
 * @param handle Handle
 * @return Handle value in uint
 */
template <class THandle>
constexpr inline uint32_t castHandleToUint(THandle handle) {
  static_assert(
      IsAnySame<THandle, ShaderHandle, BufferHandle, TextureHandle,
                TextureViewHandle, RenderPassHandle, FramebufferHandle,
                PipelineHandle, DescriptorLayoutHandle, DescriptorHandle>,
      "Type must be a render handle");

  return static_cast<uint32_t>(handle);
}

} // namespace liquid::rhi
