#pragma once

#include <vulkan/vulkan.h>
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
  DrawIndexed,
  Unknown
};

struct RenderCommandBase {
  RenderCommandType type = RenderCommandType::Unknown;
};

template <RenderCommandType commandType>
struct TypedRenderCommandBase : public RenderCommandBase {
  static_assert(commandType != RenderCommandType::Unknown);

  TypedRenderCommandBase() { type = commandType; }
};

struct RenderCommandBeginRenderPass
    : public TypedRenderCommandBase<RenderCommandType::BeginRenderPass> {
  VkRenderPass renderPass = nullptr;
  VkFramebuffer framebuffer = nullptr;
  glm::ivec2 renderAreaOffset{};
  glm::uvec2 renderAreaSize{};
  std::vector<VkClearValue> clearValues;
};

struct RenderCommandEndRenderPass
    : public TypedRenderCommandBase<RenderCommandType::EndRenderPass> {};

struct RenderCommandBindPipeline
    : public TypedRenderCommandBase<RenderCommandType::BindPipeline> {
  VkPipeline pipeline = nullptr;
  VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
};

struct RenderCommandBindDescriptorSets
    : public TypedRenderCommandBase<RenderCommandType::BindDescriptorSets> {
  VkPipelineLayout pipelineLayout = nullptr;
  VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
  uint32_t firstSet = 0;
  std::vector<VkDescriptorSet> descriptorSets;
  std::vector<uint32_t> dynamicOffsets;
};

struct RenderCommandPushConstants
    : public TypedRenderCommandBase<RenderCommandType::PushConstants> {
  VkPipelineLayout pipelineLayout = nullptr;
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
  HardwareBuffer *buffer = nullptr;
};

struct RenderCommandBindIndexBuffer
    : public TypedRenderCommandBase<RenderCommandType::BindIndexBuffer> {
  HardwareBuffer *buffer = nullptr;
  VkIndexType indexType = VK_INDEX_TYPE_MAX_ENUM;
};

struct RenderCommandDrawIndexed
    : public TypedRenderCommandBase<RenderCommandType::DrawIndexed> {
  size_t indexCount = 0;
  uint32_t firstIndex = 0;
  int32_t vertexOffset = 0;
};

} // namespace liquid
