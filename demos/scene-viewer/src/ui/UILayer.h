#pragma once

#include "core/Base.h"
#include "entity/Entity.h"
#include "entity/EntityContext.h"
#include "SceneHierarchy.h"
#include "EntityPropertyPanel.h"
#include "MenuBar.h"

class UILayer {
public:
  UILayer(liquid::EntityContext &context);

  inline SceneHierarchy &getSceneHierarchy() { return sceneHierarchy; }

  void render();

  void onSceneOpen(const MenuBar::MenuItemHandler &handler) {
    menuBar.onSceneOpen(handler);
  }

  void onEnvironmentOpen(const MenuBar::MenuItemHandler &handler) {
    menuBar.onEnvironmentOpen(handler);
  }

private:
  MenuBar menuBar;
  SceneHierarchy sceneHierarchy;
  EntityPropertyPanel entityProperties;
};
