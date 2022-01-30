#pragma once

#include "liquid/core/Base.h"
#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityContext.h"

class EntityPropertyPanel {
public:
  EntityPropertyPanel(liquid::EntityContext &context);

  void setEntity(liquid::Entity entity);

  void render();

private:
  void renderTransformDetails();

  void renderLightDetails();

private:
  liquid::EntityContext &context;
  bool panelOpen = true;

  liquid::Entity entity = std::numeric_limits<liquid::Entity>::max();
};
