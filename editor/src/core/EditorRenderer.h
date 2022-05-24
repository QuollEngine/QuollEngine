#pragma once

#include "liquid/rhi/RenderGraph.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/ShaderLibrary.h"
#include "../editor-scene/EditorGrid.h"
#include "../ui/IconRegistry.h"

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
   */
  EditorRenderer(liquid::rhi::ResourceRegistry &registry,
                 liquid::ShaderLibrary &shaderLibrary,
                 IconRegistry &iconRegistry);

  /**
   * @brief Attach to render graph
   *
   * @param graph Render graph
   * @return Newly created render pass
   */
  liquid::rhi::RenderGraphPass &attach(liquid::rhi::RenderGraph &graph);

  /**
   * @brief Update internal buffers
   *
   * @param entityContext Entity context
   * @param camera Camerae
   * @param editorGrid Editor grid
   */
  void update(liquid::EntityContext &entityContext, liquid::Entity camera,
              const EditorGrid &editorGrid);

private:
  EditorRendererStorage mRenderStorage;
  liquid::rhi::ResourceRegistry &mRegistry;
  liquid::ShaderLibrary mShaderLibrary;
  IconRegistry &mIconRegistry;
};

} // namespace liquidator
