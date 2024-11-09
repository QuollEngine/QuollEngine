#include "quoll/core/Base.h"
#include "Qui.h"
#include <imgui.h>

namespace qui {

namespace {

void handleEvents(EventManager *events) {
  auto imguiPos = ImGui::GetMousePos();
  auto pos = glm::vec2{imguiPos.x, imguiPos.y};

  if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    const MouseEvent ev{pos};
    for (const auto &handler : events->getMouseDownHandlers()) {
      handler(ev);
    }
  }

  if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
    const MouseEvent ev{pos};
    for (const auto &handler : events->getMouseUpHandlers()) {
      handler(ev);
    }
  }

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    const MouseEvent ev{pos};
    for (const auto &handler : events->getMouseClickHandlers()) {
      handler(ev);
    }
  }

  {
    const MouseEvent ev{pos};
    for (const auto &handler : events->getMouseMoveHandlers()) {
      handler(ev);
    }
  }

  {
    const auto deltaX = ImGui::GetIO().MouseWheelH;
    const auto deltaY = ImGui::GetIO().MouseWheel;

    if (deltaY != 0.0f || deltaX != 0.0f) {
      const MouseWheelEvent ev{.delta = {deltaX, deltaY}};
      for (const auto &handler : events->getMouseWheelHandlers()) {
        handler(ev);
      }
    }
  }
}

} // namespace

Tree Qui::createTree(Element &&element) {
  Tree tree{.root = std::move(element)};

  BuildContext context{tree.events.get()};
  tree.root.build(context);

  return std::move(tree);
}

void Qui::render(Tree &tree, glm::vec2 pos, glm::vec2 size) {
  const Constraints constraints(0.0f, 0.0f, size.x, size.y);
  const LayoutInput input{constraints, pos};

  handleEvents(tree.events.get());

  auto output = tree.root.getView()->layout(input);

  tree.root.getView()->render();
}

} // namespace qui
