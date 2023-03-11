#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(ActionExecutor &actionExecutor, EntityManager &entityManager,
               AssetLoader &assetLoader)
    : mActionExecutor(actionExecutor), mAssetBrowser(assetLoader),
      mSceneHierarchyPanel(entityManager), mEntityPanel(entityManager) {}

void UIRoot::render(WorkspaceState &state, EditorManager &editorManager,
                    Renderer &renderer, AssetManager &assetManager,
                    PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mLayout.setup();

  mSceneHierarchyPanel.render(state, mActionExecutor, editorManager);

  if (state.selectedEntity != Entity::Null) {
    mEntityPanel.render(editorManager, state.selectedEntity, renderer,
                        assetManager, physicsSystem);
  }

  EnvironmentPanel::render(editorManager, assetManager);

  mEditorCameraPanel.render(state, mActionExecutor);
  mAssetBrowser.render(assetManager, mIconRegistry, state, editorManager,
                       entityManager);
}

} // namespace liquid::editor
