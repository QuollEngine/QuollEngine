#include "liquid/core/Base.h"
#include "SceneRenderer.h"
#include "StandardPushConstants.h"

namespace liquid {

SceneRenderer::SceneRenderer(EntityContext &entityContext)
    : mEntityContext(entityContext) {}

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline,
                           bool bindMaterialData) {
  mEntityContext.iterateEntities<MeshComponent, TransformComponent>(
      [&commandList, &pipeline, bindMaterialData,
       this](Entity entity, const MeshComponent &mesh,
             const TransformComponent &transform) {
        if (mEntityContext.hasComponent<EnvironmentComponent>(entity)) {
          return;
        }
        const auto &instance = mesh.instance;

        StandardPushConstants transformConstant{};
        transformConstant.modelMatrix = transform.worldTransform;

        commandList.pushConstants(pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                  sizeof(StandardPushConstants),
                                  &transformConstant);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          commandList.bindVertexBuffer(instance->getVertexBuffers().at(i));

          if (instance->getMaterials().at(i) && bindMaterialData) {
            commandList.bindDescriptor(
                pipeline, 2, instance->getMaterials().at(i)->getDescriptor());
          }

          if (rhi::isHandleValid(instance->getIndexBuffers().at(i))) {
            commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                        VK_INDEX_TYPE_UINT32);
            commandList.drawIndexed(instance->getIndexCounts().at(i), 0, 0);
          } else {
            commandList.draw(instance->getVertexCounts().at(i), 0);
          }
        }
      });
}

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline,
                           RenderStorage &renderStorage,
                           AssetRegistry &assetRegistry,
                           bool bindMaterialData) {
  rhi::Descriptor descriptor;
  descriptor.bind(0, renderStorage.getMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  for (auto &[handle, meshData] : renderStorage.getMeshGroups()) {
    const auto &mesh = assetRegistry.getMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g));
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g));
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g),
                                    VK_INDEX_TYPE_UINT32);
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      const auto &material = meshData.materials.at(g);

      for (auto index : meshData.indices) {
        if (bindMaterialData) {
          commandList.bindDescriptor(pipeline, 3, material->getDescriptor());
        }

        if (indexed) {
          commandList.drawIndexed(indexCount, 0, 0, 1, index);
        } else {
          commandList.draw(vertexCount, 0, 1, index);
        }
      }
    }
  }
}

void SceneRenderer::renderSkinned(rhi::RenderCommandList &commandList,
                                  rhi::PipelineHandle pipeline,
                                  RenderStorage &renderStorage,
                                  AssetRegistry &assetRegistry,
                                  bool bindMaterialData) {
  rhi::Descriptor descriptor;
  descriptor.bind(0, renderStorage.getSkinnedMeshTransformsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  descriptor.bind(1, renderStorage.getSkeletonsBuffer(),
                  rhi::DescriptorType::StorageBuffer);
  commandList.bindDescriptor(pipeline, 1, descriptor);

  uint32_t index = 0;

  for (auto &[handle, meshData] : renderStorage.getSkinnedMeshGroups()) {
    const auto &mesh = assetRegistry.getSkinnedMeshes().getAsset(handle).data;
    for (size_t g = 0; g < mesh.vertexBuffers.size(); ++g) {
      commandList.bindVertexBuffer(mesh.vertexBuffers.at(g));
      bool indexed = rhi::isHandleValid(mesh.indexBuffers.at(g));
      if (indexed) {
        commandList.bindIndexBuffer(mesh.indexBuffers.at(g),
                                    VK_INDEX_TYPE_UINT32);
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      const auto &material = meshData.materials.at(g);

      for (auto index : meshData.indices) {
        if (bindMaterialData) {
          commandList.bindDescriptor(pipeline, 3, material->getDescriptor());
        }

        if (indexed) {
          commandList.drawIndexed(indexCount, 0, 0, 1, index);
        } else {
          commandList.draw(vertexCount, 0, 1, index);
        }
      }
    }
  }
}

} // namespace liquid
