#pragma once

#include "liquid/rhi/RenderGraph.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquidator/ui/IconRegistry.h"

#include "EditorRendererStorage.h"

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
   * @param registry Resource registry
   * @param shaderLibrary Shader library
   * @param iconRegistry Icon registry
   * @param device Render device
   */
  EditorRenderer(liquid::rhi::ResourceRegistry &registry,
                 liquid::ShaderLibrary &shaderLibrary,
                 IconRegistry &iconRegistry, liquid::rhi::RenderDevice *device);

  /**
   * @brief Attach to render graph
   *
   * @param graph Render graph
   * @return Newly created render pass
   */
  liquid::rhi::RenderGraphPass &attach(liquid::rhi::RenderGraph &graph);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera
   * @param editorGrid Editor grid
   * @param selectedEntity Selected entity
   */
  void updateFrameData(liquid::EntityDatabase &entityDatabase,
                       liquid::Entity camera, const EditorGrid &editorGrid,
                       liquid::Entity selectedEntity);

private:
  /**
   * @brief Create buffers for collidable shapes
   */
  void createCollidableShapes();

private:
  EditorRendererStorage mRenderStorage;
  liquid::rhi::ResourceRegistry &mRegistry;
  liquid::rhi::RenderDevice *mDevice;
  liquid::ShaderLibrary mShaderLibrary;
  IconRegistry &mIconRegistry;
  liquid::Entity mSelectedEntity = liquid::EntityNull;

  CollidableShapeDraw mCollidableCube;
  CollidableShapeDraw mCollidableSphere;
  CollidableShapeDraw mCollidableCapsule;
};

} // namespace liquidator
