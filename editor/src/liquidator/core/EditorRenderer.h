#pragma once

#include "liquid/renderer/RenderGraph.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquidator/ui/IconRegistry.h"

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
   * @param shaderLibrary Shader library
   * @param iconRegistry Icon registry
   * @param renderStorage Render storage
   * @param device Render device
   */
  EditorRenderer(ShaderLibrary &shaderLibrary, IconRegistry &iconRegistry,
                 RenderStorage &renderStorage, rhi::RenderDevice *device);

  /**
   * @brief Attach to render graph
   *
   * @param graph Render graph
   * @return Newly created render pass
   */
  RenderGraphPass &attach(RenderGraph &graph);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera
   * @param editorGrid Editor grid
   * @param selectedEntity Selected entity
   * @param frameIndex Frame index
   */
  void updateFrameData(EntityDatabase &entityDatabase, Entity camera,
                       const EditorGrid &editorGrid, Entity selectedEntity,
                       uint32_t frameIndex);

private:
  /**
   * @brief Create buffers for collidable shapes
   */
  void createCollidableShapes();

private:
  rhi::RenderDevice *mDevice;
  ShaderLibrary mShaderLibrary;
  IconRegistry &mIconRegistry;
  Entity mSelectedEntity = Entity::Null;

  CollidableShapeDraw mCollidableCube;
  CollidableShapeDraw mCollidableSphere;
  CollidableShapeDraw mCollidableCapsule;

  RenderStorage &mRenderStorage;
  std::array<EditorRendererFrameData, rhi::RenderDevice::NumFrames> mFrameData;
};

} // namespace liquid::editor
