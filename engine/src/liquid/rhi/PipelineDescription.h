#pragma once

#include "liquid/renderer/render-graph/RenderGraphPipelineDescription.h"

namespace liquid::rhi {

struct PipelineDescription {
  RenderPassHandle renderPass = 0;
  ShaderHandle vertexShader = 0;
  ShaderHandle fragmentShader = 0;
  PipelineVertexInputLayout inputLayout;
  PipelineInputAssembly inputAssembly;
  PipelineRasterizer rasterizer;
  PipelineColorBlend colorBlend;
};

} // namespace liquid::rhi
