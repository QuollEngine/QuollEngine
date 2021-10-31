#pragma once

#include <vulkan/vulkan.hpp>
#include "HardwareBuffer.h"

namespace liquid {

enum class RenderCommandType {
  BeginRenderPass,
  EndRenderPass,
  SetViewport,
  SetScissor,
  BindPipeline,
  BindDescriptorSets,
  PushConstants,
  BindVertexBuffer,
  BindIndexBuffer,
  Draw,
  DrawIndexed,
  Unknown
};

struct RenderCommandBase {
  RenderCommandType type = RenderCommandType::Unknown;

  /**
   * @brief Create render command
   *
   * @param type Render command type
   */
  RenderCommandBase(RenderCommandType type_) : type(type_){};

  /**
   * @brief Delete render command
   */
  virtual ~RenderCommandBase() = default;

  RenderCommandBase(RenderCommandBase &rhs) = delete;
  RenderCommandBase(RenderCommandBase &&rhs) = delete;
  RenderCommandBase &operator=(RenderCommandBase &rhs) = delete;
  RenderCommandBase &operator=(RenderCommandBase &&rhs) = delete;
};

template <RenderCommandType commandType>
struct TypedRenderCommandBase : public RenderCommandBase {
  static_assert(commandType != RenderCommandType::Unknown);

  /**
   * @brief Create typed render command
   *
   * Template enum parameter will be used to create
   * command type
   */
  TypedRenderCommandBase() : RenderCommandBase(commandType) {}
};

struct RenderCommandBeginRenderPass
    : public TypedRenderCommandBase<RenderCommandType::BeginRenderPass> {
  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  glm::ivec2 renderAreaOffset{};
  glm::uvec2 renderAreaSize{};
  std::vector<VkClearValue> clearValues;
};

struct RenderCommandEndRenderPass
    : public TypedRenderCommandBase<RenderCommandType::EndRenderPass> {};

struct RenderCommandBindPipeline
    : public TypedRenderCommandBase<RenderCommandType::BindPipeline> {
  VkPipeline pipeline = VK_NULL_HANDLE;
  VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
};

struct RenderCommandBindDescriptorSets
    : public TypedRenderCommandBase<RenderCommandType::BindDescriptorSets> {
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
  uint32_t firstSet = 0;
  std::vector<VkDescriptorSet> descriptorSets;
  std::vector<uint32_t> dynamicOffsets;
};

struct RenderCommandPushConstants
    : public TypedRenderCommandBase<RenderCommandType::PushConstants> {
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkShaderStageFlags stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  uint32_t offset = 0;
  uint32_t size = 0;
  std::unique_ptr<char> data;
};

struct RenderCommandSetViewport
    : public TypedRenderCommandBase<RenderCommandType::SetViewport> {
  glm::vec2 offset{};
  glm::vec2 size{};
  glm::vec2 depthRange{};
};

struct RenderCommandSetScissor
    : public TypedRenderCommandBase<RenderCommandType::SetScissor> {
  glm::ivec2 offset{};
  glm::uvec2 size{};
};

struct RenderCommandBindVertexBuffer
    : public TypedRenderCommandBase<RenderCommandType::BindVertexBuffer> {
  SharedPtr<HardwareBuffer> buffer = nullptr;
};

struct RenderCommandBindIndexBuffer
    : public TypedRenderCommandBase<RenderCommandType::BindIndexBuffer> {
  SharedPtr<HardwareBuffer> buffer = nullptr;
  VkIndexType indexType = VK_INDEX_TYPE_MAX_ENUM;
};

struct RenderCommandDraw
    : public TypedRenderCommandBase<RenderCommandType::Draw> {
  size_t vertexCount = 0;
  uint32_t firstVertex = 0;
};

struct RenderCommandDrawIndexed
    : public TypedRenderCommandBase<RenderCommandType::DrawIndexed> {
  size_t indexCount = 0;
  uint32_t firstIndex = 0;
  int32_t vertexOffset = 0;
};

} // namespace liquid
