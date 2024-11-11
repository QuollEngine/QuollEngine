#include "quoll/core/Base.h"
#include "ImGuiEventProcessorBackend.h"
#include <imgui.h>

namespace qui {

glm::vec2 ImGuiEventProcessorBackend::getMousePosition() {
  auto imguiPos = ImGui::GetMousePos();
  return {imguiPos.x, imguiPos.y};
}

glm::vec2 ImGuiEventProcessorBackend::getMouseWheelDelta() {
  return {ImGui::GetIO().MouseWheelH, ImGui::GetIO().MouseWheel};
}

bool ImGuiEventProcessorBackend::isMouseClicked() {
  return ImGui::IsMouseClicked(ImGuiMouseButton_Left);
}

bool ImGuiEventProcessorBackend::isMouseDown() {
  return ImGui::IsMouseDown(ImGuiMouseButton_Left);
}

bool ImGuiEventProcessorBackend::isMouseUp() {
  return ImGui::IsMouseReleased(ImGuiMouseButton_Left);
}

bool ImGuiEventProcessorBackend::isMouseMoved() {
  const auto mousePosition = getMousePosition();
  const bool isMoved = mousePosition != mPreviousMousePosition;
  mPreviousMousePosition = mousePosition;
  return isMoved;
}

bool ImGuiEventProcessorBackend::isMouseWheel() {
  const auto wheel = getMouseWheelDelta();
  return wheel.x != 0.0f || wheel.y != 0.0f;
}

} // namespace qui
