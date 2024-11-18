#include "quoll/core/Base.h"
#include "component/EventProcessor.h"
#include "component/ImGuiEventProcessorBackend.h"
#include "Qui.h"

namespace qui {

Tree Qui::createTree(Element &&element) {
  Tree tree{.root = std::move(element)};

  BuildContext context{tree.globalEvents.get()};
  tree.root.build(context);

  return std::move(tree);
}

void Qui::render(Tree &tree, glm::vec2 pos, glm::vec2 size) {
  const Constraints constraints(0.0f, 0.0f, size.x, size.y);
  const LayoutInput input{constraints, pos};

  static EventProcessor<ImGuiEventProcessorBackend> eventProcessor;

  eventProcessor.processEvents(tree);

  tree.root.getView()->layout(input);
  tree.root.getView()->render();
}

} // namespace qui
