#pragma once

#include "liquid/rhi/PipelineDescription.h"

namespace quoll {

namespace mesh_vertex_layout_detail {

template <class TType> static rhi::Format getFormat() {
  if constexpr (std::is_same_v<TType, glm::vec2>) {
    return rhi::Format::Rg32Float;
  }

  if constexpr (std::is_same_v<TType, glm::vec3>) {
    return rhi::Format::Rgb32Float;
  }

  if constexpr (std::is_same_v<TType, glm::vec4>) {
    return rhi::Format::Rgba32Float;
  }

  if constexpr (std::is_same_v<TType, glm::uvec4>) {
    return rhi::Format::Rgba32Uint;
  }

  static_assert("Format not defined");
}

template <typename... TTypes>
static constexpr void
createBindings(std::vector<rhi::PipelineVertexInputBinding> &bindings) {
  (bindings.push_back({.binding = static_cast<uint32_t>(bindings.size()),
                       .stride = sizeof(TTypes),
                       .inputRate = rhi::VertexInputRate::Vertex}),
   ...);
}

template <typename... TTypes>
static constexpr void
createAttributes(std::vector<rhi::PipelineVertexInputAttribute> &attributes) {
  (attributes.push_back({.slot = static_cast<uint32_t>(attributes.size()),
                         .binding = static_cast<uint32_t>(attributes.size()),
                         .format = getFormat<TTypes>(),
                         .offset = 0}),
   ...);
}

} // namespace mesh_vertex_layout_detail

template <typename... TTypes>
static constexpr rhi::PipelineVertexInputLayout createMeshVertexLayoutFrom() {
  rhi::PipelineVertexInputLayout layout{};

  mesh_vertex_layout_detail::createBindings<TTypes...>(layout.bindings);
  mesh_vertex_layout_detail::createAttributes<TTypes...>(layout.attributes);

  return layout;
}

static rhi::PipelineVertexInputLayout createMeshPositionLayout() {
  return createMeshVertexLayoutFrom<glm::vec3>();
}

static rhi::PipelineVertexInputLayout createMeshVertexLayout() {
  return createMeshVertexLayoutFrom<glm::vec3, glm::vec3, glm::vec4, glm::vec2,
                                    glm::vec2>();
}

static rhi::PipelineVertexInputLayout createSkinnedMeshVertexLayout() {
  return createMeshVertexLayoutFrom<glm::vec3, glm::vec3, glm::vec4, glm::vec2,
                                    glm::vec2, glm::uvec4, glm::vec4>();
}

static rhi::PipelineVertexInputLayout createSkinnedMeshPositionLayout() {
  return createMeshVertexLayoutFrom<glm::vec3, glm::uvec4, glm::vec4>();
}

} // namespace quoll
