#pragma once

#include "liquid/core/Base.h"

class MenuBar {
public:
  using MenuItemHandler = std::function<void()>;

public:
  void render();

  inline void onSceneOpen(const MenuItemHandler &handler) {
    sceneOpenHandler = handler;
  }

  inline void onEnvironmentOpen(const MenuItemHandler &handler) {
    environmentOpenHandler = handler;
  }

private:
  MenuItemHandler sceneOpenHandler;
  MenuItemHandler environmentOpenHandler;
};
