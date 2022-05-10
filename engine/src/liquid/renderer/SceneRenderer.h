#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/asset/AssetRegistry.h"
#include "RenderStorage.h"

namespace liquid {

/**
 * @brief Scene renderer
 */
class SceneRenderer {
public:
  /**
   * @brief Render meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param renderStorage Render storage
   * @param assetRegistry Asset registry
   * @param bindMaterialData Bind material data
   */
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              const RenderStorage &renderStorage, AssetRegistry &assetRegistry,
              bool bindMaterialData = false);

  /**
   * @brief Render skinned meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param renderStorage Render storage
   * @param assetRegistry Asset registry
   * @param bindMaterialData Bind material data
   */
  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline,
                     const RenderStorage &renderStorage,
                     AssetRegistry &assetRegistry,
                     bool bindMaterialData = true);
};

} // namespace liquid
