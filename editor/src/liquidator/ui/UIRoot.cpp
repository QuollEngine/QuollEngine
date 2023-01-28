#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "UIRoot.h"

namespace liquid::editor {

UIRoot::UIRoot(EntityManager &entityManager, AssetLoader &assetLoader)
    : mAssetBrowser(assetLoader), mSceneHierarchyPanel(entityManager),
      mEntityPanel(entityManager) {}

void UIRoot::render(EditorManager &editorManager, Renderer &renderer,
                    AssetManager &assetManager, PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mLayout.setup();

  mSceneHierarchyPanel.render(editorManager);

  if (mSceneHierarchyPanel.isEntitySelected()) {
    mEntityPanel.render(editorManager, mSceneHierarchyPanel.getSelectedEntity(),
                        renderer, assetManager, physicsSystem);
  }

  EnvironmentPanel::render(editorManager, assetManager);

  mEditorCameraPanel.render(editorManager);
  mAssetBrowser.render(assetManager, mIconRegistry, editorManager,
                       entityManager);
}

} // namespace liquid::editor
