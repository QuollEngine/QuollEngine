#include "liquid/core/Base.h"
#include "TransformOperationControl.h"
#include "StyleStack.h"

#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

#include "Theme.h"

namespace liquidator {

TransformOperationControl::TransformOperationControl(
    const IconRegistry &iconRegistry, EditorManager &editorManager,
    float iconSize) {

  renderIcon(iconRegistry, TransformOperation::Move, editorManager, iconSize);
  ImGui::SameLine();
  renderIcon(iconRegistry, TransformOperation::Rotate, editorManager, iconSize);
  ImGui::SameLine();
  renderIcon(iconRegistry, TransformOperation::Scale, editorManager, iconSize);
}

void TransformOperationControl::renderIcon(
    const IconRegistry &iconRegistry, TransformOperation transformOperation,
    EditorManager &editorManager, float iconSize) {

  StyleStack stack;
  if (transformOperation == editorManager.getTransformOperation()) {
    const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
    glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

    stack.pushColor(ImGuiCol_Button, buttonColor);
    stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
    stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
  }

  if (liquid::imgui::imageButton(
          iconRegistry.getIcon(getTransformOperationIcon(transformOperation)),
          ImVec2(iconSize, iconSize))) {
    editorManager.setTransformOperation(transformOperation);
  }
}

EditorIcon TransformOperationControl::getTransformOperationIcon(
    TransformOperation transformOperation) {
  switch (transformOperation) {
  case TransformOperation::Scale:
    return EditorIcon::Scale;
  case TransformOperation::Rotate:
    return EditorIcon::Rotate;
  case TransformOperation::Move:
  default:
    return EditorIcon::Move;
  }
}

} // namespace liquidator
