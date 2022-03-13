#pragma once

#include "liquid/renderer/Shader.h"
#include "liquid/renderer/render-graph/RenderGraphPipelineDescription.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

struct PipelineDescription {
  RenderPassHandle renderPass = 0;
  SharedPtr<Shader> vertexShader;
  SharedPtr<Shader> fragmentShader;
  PipelineVertexInputLayout inputLayout;
  PipelineInputAssembly inputAssembly;
  PipelineRasterizer rasterizer;
  PipelineColorBlend colorBlend;
};

} // namespace liquid
