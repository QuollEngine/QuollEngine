#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(liquid::EntityContext &entityContext,
               const liquid::GLTFLoader &gltfLoader, GLTFImporter &gltfImporter)
    : mMenuBar(gltfLoader, gltfImporter), mSceneHierarchyPanel(entityContext),
      mEntityPanel(entityContext) {
  mSceneHierarchyPanel.setNodeClickHandler(
      [this](liquid::SceneNode *node) { handleNodeClick(node); });
}

void UIRoot::render(SceneManager &sceneManager,
                    const liquid::AnimationSystem &animationSystem,
                    liquid::PhysicsSystem &physicsSystem) {
  mLayout.setup();
  mMenuBar.render(sceneManager);
  mStatusBar.render(sceneManager);
  mSceneHierarchyPanel.render(sceneManager);
  mEntityPanel.render(sceneManager, animationSystem, physicsSystem);
  mEditorCameraPanel.render(sceneManager);
}

void UIRoot::handleNodeClick(liquid::SceneNode *node) {
  mEntityPanel.setSelectedEntity(node->getEntity());
}

} // namespace liquidator
