#pragma once

#include "liquid/renderer/render-graph/RenderGraphPipelineDescription.h"

namespace liquid::rhi {

struct PipelineDescription {
  RenderPassHandle renderPass = RenderPassHandle::Invalid;
  ShaderHandle vertexShader = ShaderHandle::Invalid;
  ShaderHandle fragmentShader = ShaderHandle::Invalid;
  PipelineVertexInputLayout inputLayout;
  PipelineInputAssembly inputAssembly;
  PipelineRasterizer rasterizer;
  PipelineColorBlend colorBlend;
};

} // namespace liquid::rhi
