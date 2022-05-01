#include "liquid/core/Base.h"
#include "SceneRenderer.h"
#include "StandardPushConstants.h"

namespace liquid {

void SceneRenderer::render(rhi::RenderCommandList &commandList,
                           rhi::PipelineHandle pipeline,
                           const RenderStorage &renderStorage,
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

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
      }

      uint32_t indexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).indices.size());
      uint32_t vertexCount =
          static_cast<uint32_t>(mesh.geometries.at(g).vertices.size());

      for (auto index : meshData.indices) {
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
                                  const RenderStorage &renderStorage,
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

      if (bindMaterialData) {
        commandList.bindDescriptor(pipeline, 3,
                                   mesh.materials.at(g)->getDescriptor());
      }

      for (auto index : meshData.indices) {
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
