#pragma once

#include "liquid/renderer/RenderGraph.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquidator/state/WorkspaceState.h"
#include "liquid/renderer/SceneRenderer.h"

#include "EditorRendererFrameData.h"

namespace liquid::editor {

/**
 * @brief Editor renderer
 *
 * Creates editor shaders and render pass
 */
class EditorRenderer {
  /**
   * @brief Collidable shape draw information
   */
  struct CollidableShapeDraw {
    /**
     * @brief Vertex buffer that holds shape data
     */
    rhi::Buffer buffer;

    /**
     * @brief Number of vertices
     *
     * Used when recording draw command
     */
    uint32_t vertexCount = 0;
  };

public:
  /**
   * @brief Create editor renderer
   *
   * @param renderStorage Render storage
   * @param device Render device
   */
  EditorRenderer(RenderStorage &renderStorage, rhi::RenderDevice *device);

  /**
   * @brief Attach to render graph
   *
   * @param graph Render graph
   * @param scenePassData Scene pass data
   * @param options Renderer options
   */
  void attach(RenderGraph &graph, const SceneRenderPassData &scenePassData,
              const RendererOptions &options);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @param frameIndex Frame index
   */
  void updateFrameData(EntityDatabase &entityDatabase, Entity camera,
                       WorkspaceState &state, AssetRegistry &assetRegistry,
                       uint32_t frameIndex);

private:
  void renderSpriteOutlines(rhi::RenderCommandList &commandList,
                            EditorRendererFrameData &frameData,
                            rhi::PipelineHandle pipeline,
                            uint32_t instanceStart, uint32_t instanceEnd,
                            glm::vec4 color, float scale);

  void renderTextOutlines(rhi::RenderCommandList &commandList,
                          EditorRendererFrameData &frameData,
                          rhi::PipelineHandle pipeline, uint32_t instanceStart,
                          uint32_t instanceEnd, glm::vec4 color, float scale);

  void renderMeshOutlines(rhi::RenderCommandList &commandList,
                          EditorRendererFrameData &frameData,
                          rhi::PipelineHandle pipeline, uint32_t instanceStart,
                          uint32_t instanceEnd, glm::vec4 color, float scale);

  /**
   * @brief Create buffers for collidable shapes
   */
  void createCollidableShapes();

private:
  rhi::RenderDevice *mDevice;
  Entity mSelectedEntity = Entity::Null;

  CollidableShapeDraw mCollidableCube;
  CollidableShapeDraw mCollidableSphere;
  CollidableShapeDraw mCollidableCapsule;

  RenderStorage &mRenderStorage;
  std::array<EditorRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;
};

} // namespace liquid::editor
