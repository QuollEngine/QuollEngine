#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(EntityManager &entityManager, AssetLoader &assetLoader)
    : mAssetBrowser(assetLoader), mSceneHierarchyPanel(entityManager),
      mEntityPanel(entityManager) {}

void UIRoot::render(WorkspaceState &state, EditorManager &editorManager,
                    Renderer &renderer, AssetManager &assetManager,
                    PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mLayout.setup();

  mSceneHierarchyPanel.render(state, editorManager);

  if (state.selectedEntity != Entity::Null) {
    mEntityPanel.render(editorManager, state.selectedEntity, renderer,
                        assetManager, physicsSystem);
  }

  EnvironmentPanel::render(editorManager, assetManager);

  mEditorCameraPanel.render(state);
  mAssetBrowser.render(assetManager, mIconRegistry, state, editorManager,
                       entityManager);
}

} // namespace liquid::editor
