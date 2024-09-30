#include "quoll/core/Base.h"
#include "Qui.h"
#include <imgui.h>

namespace qui {

Tree Qui::createTree(Element &&element) {
  Tree tree{.root = std::move(element)};
  tree.root.build();

  return tree;
}

void Qui::render(Tree &tree, glm::vec2 pos, glm::vec2 size) {
  Constraints constraints(0.0f, 0.0f, size.x, size.y);
  LayoutInput input{constraints, pos};

  auto output = tree.root.getView()->layout(input);
  tree.root.getView()->render();
}

} // namespace qui
