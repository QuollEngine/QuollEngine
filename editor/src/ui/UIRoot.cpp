#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(liquid::EntityContext &entityContext,
               EntityManager &entityManager, GLTFImporter &gltfImporter)
    : mAssetBrowser(gltfImporter), mSceneHierarchyPanel(entityContext),
      mEntityPanel(entityContext, entityManager) {
  mSceneHierarchyPanel.setNodeClickHandler(
      [this](liquid::SceneNode *node) { handleNodeClick(node); });
}

void UIRoot::render(SceneManager &sceneManager, liquid::Renderer &renderer,
                    liquid::AssetManager &assetManager,
                    liquid::PhysicsSystem &physicsSystem) {
  mLayout.setup();
  mMenuBar.render(sceneManager);
  mStatusBar.render(sceneManager);
  mSceneHierarchyPanel.render(sceneManager);
  mEntityPanel.render(sceneManager, renderer, assetManager.getRegistry(),
                      physicsSystem);
  mEditorCameraPanel.render(sceneManager);
  mAssetBrowser.render(assetManager, mIconRegistry, sceneManager);
}

void UIRoot::handleNodeClick(liquid::SceneNode *node) {
  mEntityPanel.setSelectedEntity(node->getEntity());
}

} // namespace liquidator
