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
   * @param camera Camerae
   * @param editorGrid Editor grid
   */
  void updateFrameData(liquid::EntityDatabase &entityDatabase,
                       liquid::Entity camera, const EditorGrid &editorGrid);

private:
  EditorRendererStorage mRenderStorage;
  liquid::rhi::ResourceRegistry &mRegistry;
  liquid::ShaderLibrary mShaderLibrary;
  IconRegistry &mIconRegistry;
};

} // namespace liquidator
