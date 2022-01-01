#include "core/Base.h"
#include "SceneRenderer.h"
#include "vulkan/VulkanStandardPushConstants.h"

namespace liquid {

SceneRenderer::SceneRenderer(EntityContext &entityContext_,
                             bool bindMaterialData_)
    : entityContext(entityContext_), bindMaterialData(bindMaterialData_) {}

void SceneRenderer::render(RenderCommandList &commandList,
                           const SharedPtr<Pipeline> &pipeline) {
  entityContext.iterateEntities<MeshComponent, TransformComponent>(
      [&commandList, &pipeline, this](Entity entity, const MeshComponent &mesh,
                                      const TransformComponent &transform) {
        if (entityContext.hasComponent<EnvironmentComponent>(entity)) {
          return;
        }
        const auto &instance = mesh.instance;

        auto *transformConstant = new VulkanStandardPushConstants;
        transformConstant->modelMatrix = transform.transformWorld;

        commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                  sizeof(VulkanStandardPushConstants),
                                  transformConstant);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          commandList.bindVertexBuffer(instance->getVertexBuffers().at(i));

          if (instance->getMaterials().at(i) && bindMaterialData) {
            commandList.bindDescriptor(
                pipeline, 2, instance->getMaterials().at(i)->getDescriptor());
          }

          if (instance->getIndexBuffers().at(i) != nullptr) {
            commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                        VK_INDEX_TYPE_UINT32);
            commandList.drawIndexed(instance->getIndexCounts().at(i), 0, 0);
          } else {
            commandList.draw(instance->getVertexCounts().at(i), 0);
          }
        }
      });
}

} // namespace liquid
