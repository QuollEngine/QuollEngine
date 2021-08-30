#include "UILayer.h"

UILayer::UILayer(liquid::EntityContext &context)
    : sceneHierarchy(context), entityProperties(context) {
  sceneHierarchy.onNodeClick(
      [this](liquid::Entity entity) { entityProperties.setEntity(entity); });
}

void UILayer::render() {
  menuBar.render();
  sceneHierarchy.render();
  entityProperties.render();
}
