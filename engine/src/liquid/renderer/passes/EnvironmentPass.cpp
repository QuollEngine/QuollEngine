#include "liquid/core/Base.h"
#include "EnvironmentPass.h"

namespace liquid {

EnvironmentPass::EnvironmentPass(const String &name,
                                 GraphResourceId renderPassId,
                                 EntityContext &entityContext_,
                                 ShaderLibrary &shaderLibrary_,
                                 const SharedPtr<VulkanRenderData> &renderData_)
    : RenderGraphPassBase(name, renderPassId), entityContext(entityContext_),
      shaderLibrary(shaderLibrary_), renderData(renderData_) {}

void EnvironmentPass::buildInternal(RenderGraphBuilder &builder) {
  pipelineId = builder.create(RenderGraphPipelineDescription{
      shaderLibrary.getShader("__engine.skybox.default.vertex"),
      shaderLibrary.getShader("__engine.skybox.default.fragment"),
      PipelineVertexInputLayout::create<Vertex>(), PipelineInputAssembly{},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise},
      PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

  builder.write("mainColor");
  builder.write("depthBuffer");
}

void EnvironmentPass::execute(RenderCommandList &commandList,
                              RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  Descriptor descriptor;
  descriptor.bind(0,
                  renderData->getScene()->getActiveCamera()->getUniformBuffer(),
                  DescriptorType::UniformBuffer);

  commandList.bindDescriptor(pipeline, 0, descriptor);

  entityContext.iterateEntities<EnvironmentComponent, MeshComponent>(
      [this, &pipeline, &commandList](Entity entity,
                                      const EnvironmentComponent &,
                                      const MeshComponent &mesh) {
        for (size_t i = 0; i < mesh.instance->getVertexBuffers().size(); ++i) {
          commandList.bindVertexBuffer(mesh.instance->getVertexBuffers().at(i));
          commandList.bindDescriptor(
              pipeline, 1,
              mesh.instance->getMaterials().at(i)->getDescriptor());

          if (mesh.instance->getIndexBuffers().at(i) > 0) {
            commandList.bindIndexBuffer(mesh.instance->getIndexBuffers().at(i),
                                        VK_INDEX_TYPE_UINT32);
            commandList.drawIndexed(mesh.instance->getIndexCounts().at(i), 0,
                                    0);
          } else {
            commandList.draw(mesh.instance->getVertexCounts().at(i), 0);
          }
        }
      });
}

} // namespace liquid
