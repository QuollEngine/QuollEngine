#pragma once

#include "component/Tree.h"

namespace qui {

class Qui {
public:
  static Tree createTree(Element &&element);

  static void render(Tree &tree, glm::vec2 pos, glm::vec2 size);
};

} // namespace qui
