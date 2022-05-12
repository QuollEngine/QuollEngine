#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(EntityManager &entityManager, GLTFImporter &gltfImporter)
    : mAssetBrowser(gltfImporter), mSceneHierarchyPanel(entityManager),
      mEntityPanel(entityManager) {
  mSceneHierarchyPanel.setEntityClickHandler([this](liquid::Entity entity) {
    mEntityPanel.setSelectedEntity(entity);
  });
}

void UIRoot::render(SceneManager &sceneManager, liquid::Renderer &renderer,
                    liquid::AssetManager &assetManager,
                    liquid::PhysicsSystem &physicsSystem,
                    EntityManager &entityManager) {
  mLayout.setup();
  mMenuBar.render(sceneManager, entityManager);
  mStatusBar.render(sceneManager);
  mSceneHierarchyPanel.render(sceneManager);

  mInspector.render(
      [&sceneManager, &renderer, &assetManager, &physicsSystem, this]() {
        mEntityPanel.render(sceneManager, renderer, assetManager.getRegistry(),
                            physicsSystem);
        mEnvironmentPanel.render(sceneManager, assetManager);
      });

  mEditorCameraPanel.render(sceneManager);
  mAssetBrowser.render(assetManager, mIconRegistry, sceneManager,
                       entityManager);
}

} // namespace liquidator
