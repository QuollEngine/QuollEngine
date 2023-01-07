#pragma once

#include "liquid/renderer/RenderGraph.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquidator/ui/IconRegistry.h"

#include "EditorRendererFrameData.h"

namespace liquidator {

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
    liquid::rhi::Buffer buffer;

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
  EditorRenderer(liquid::ShaderLibrary &shaderLibrary,
                 IconRegistry &iconRegistry,
                 liquid::RenderStorage &renderStorage,
                 liquid::rhi::RenderDevice *device);

  /**
   * @brief Attach to render graph
   *
   * @param graph Render graph
   * @return Newly created render pass
   */
  liquid::RenderGraphPass &attach(liquid::RenderGraph &graph);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera
   * @param editorGrid Editor grid
   * @param selectedEntity Selected entity
   * @param frameIndex Frame index
   */
  void updateFrameData(liquid::EntityDatabase &entityDatabase,
                       liquid::Entity camera, const EditorGrid &editorGrid,
                       liquid::Entity selectedEntity, uint32_t frameIndex);

private:
  /**
   * @brief Create buffers for collidable shapes
   */
  void createCollidableShapes();

private:
  liquid::rhi::RenderDevice *mDevice;
  liquid::ShaderLibrary mShaderLibrary;
  IconRegistry &mIconRegistry;
  liquid::Entity mSelectedEntity = liquid::EntityNull;

  CollidableShapeDraw mCollidableCube;
  CollidableShapeDraw mCollidableSphere;
  CollidableShapeDraw mCollidableCapsule;

  liquid::RenderStorage &mRenderStorage;
  std::array<EditorRendererFrameData, liquid::rhi::RenderDevice::NumFrames>
      mFrameData;
};

} // namespace liquidator
