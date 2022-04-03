#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(liquid::EntityContext &entityContext, GLTFImporter &gltfImporter)
    : mMenuBar(gltfImporter), mSceneHierarchyPanel(entityContext),
      mEntityPanel(entityContext) {
  mSceneHierarchyPanel.setNodeClickHandler(
      [this](liquid::SceneNode *node) { handleNodeClick(node); });
}

void UIRoot::render(SceneManager &sceneManager,
                    liquid::AssetRegistry &assetRegistry,
                    liquid::PhysicsSystem &physicsSystem) {
  mLayout.setup();
  mMenuBar.render(sceneManager);
  mStatusBar.render(sceneManager);
  mSceneHierarchyPanel.render(sceneManager);
  mEntityPanel.render(sceneManager, assetRegistry, physicsSystem);
  mEditorCameraPanel.render(sceneManager);
}

void UIRoot::handleNodeClick(liquid::SceneNode *node) {
  mEntityPanel.setSelectedEntity(node->getEntity());
}

} // namespace liquidator
