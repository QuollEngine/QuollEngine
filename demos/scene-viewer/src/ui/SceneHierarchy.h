#pragma once

#include "liquid/scene/Scene.h"

class SceneHierarchy {
  using ClickHandler = std::function<void(liquid::Entity entity)>;

public:
  SceneHierarchy(liquid::EntityContext &context);

  void setScene(const liquid::SharedPtr<liquid::Scene> &scene);

  void render();

  inline void onNodeClick(const ClickHandler &handler) {
    clickHandler = handler;
  }

private:
  void renderNode(liquid::SceneNode *node, int flags);

private:
  liquid::EntityContext &context;
  liquid::SharedPtr<liquid::Scene> scene;
  bool panelOpen = true;
  ClickHandler clickHandler;
};
