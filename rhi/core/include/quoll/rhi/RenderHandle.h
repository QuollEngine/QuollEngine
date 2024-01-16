#pragma once

namespace quoll::rhi {

enum class ShaderHandle : u32 { Null = 0 };

enum class BufferHandle : u32 { Null = 0 };

enum class TextureHandle : u32 { Null = 0 };

enum class SamplerHandle : u32 { Null = 0 };

enum class RenderPassHandle : u32 { Null = 0 };

enum class FramebufferHandle : u32 { Null = 0 };

enum class PipelineHandle : u32 { Null = 0 };

enum class DescriptorLayoutHandle : u32 { Null = 0 };

enum class DescriptorHandle : u32 { Null = 0 };

/**
 * @brief Check if type equals any of the other types
 *
 * @tparam T Type
 * @tparam ...Rest Other types
 */
template <class T, class... Rest>
inline constexpr bool IsAnySame = (std::is_same_v<T, Rest> || ...);

template <class THandle> constexpr inline bool isHandleValid(THandle handle) {
  static_assert(
      IsAnySame<THandle, ShaderHandle, BufferHandle, TextureHandle,
                SamplerHandle, RenderPassHandle, FramebufferHandle,
                PipelineHandle, DescriptorLayoutHandle, DescriptorHandle>,
      "Type must be a render handle");
  return handle != THandle::Null;
}

template <class THandle> constexpr inline u32 castHandleToUint(THandle handle) {
  static_assert(
      IsAnySame<THandle, ShaderHandle, BufferHandle, TextureHandle,
                SamplerHandle, RenderPassHandle, FramebufferHandle,
                PipelineHandle, DescriptorLayoutHandle, DescriptorHandle>,
      "Type must be a render handle");

  return static_cast<u32>(handle);
}

} // namespace quoll::rhi
