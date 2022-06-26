#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(EntityManager &entityManager, AssetLoader &assetLoader)
    : mAssetBrowser(assetLoader), mSceneHierarchyPanel(entityManager),
      mEntityPanel(entityManager) {
  mSceneHierarchyPanel.setEntityClickHandler([this](liquid::Entity entity) {
    mEntityPanel.setSelectedEntity(entity);
  });
}

void UIRoot::render(EditorManager &editorManager, liquid::Renderer &renderer,
                    liquid::AssetManager &assetManager,
                    liquid::PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mLayout.setup();
  mMenuBar.render(editorManager, entityManager);
  mStatusBar.render(editorManager);
  mSceneHierarchyPanel.render(editorManager);

  mInspector.render(
      [&editorManager, &renderer, &assetManager, &physicsSystem, this]() {
        mEntityPanel.render(editorManager, renderer, assetManager.getRegistry(),
                            physicsSystem);
        mEnvironmentPanel.render(editorManager, assetManager);
      });

  mEditorCameraPanel.render(editorManager);
  mAssetBrowser.render(assetManager, mIconRegistry, editorManager,
                       entityManager);
}

} // namespace liquidator
