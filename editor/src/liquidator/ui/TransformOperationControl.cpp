#include "liquid/core/Base.h"

#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

#include "TransformOperationControl.h"
#include "StyleStack.h"
#include "Theme.h"
#include "FontAwesome.h"

namespace liquid::editor {

TransformOperationControl::TransformOperationControl(
    EditorManager &editorManager) {

  renderIcon(TransformOperation::Move, editorManager);
  ImGui::SameLine();
  renderIcon(TransformOperation::Rotate, editorManager);
  ImGui::SameLine();
  renderIcon(TransformOperation::Scale, editorManager);
}

void TransformOperationControl::renderIcon(
    TransformOperation transformOperation, EditorManager &editorManager) {

  StyleStack stack;
  if (transformOperation == editorManager.getTransformOperation()) {
    const auto &imguiCol = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
    glm::vec4 buttonColor{imguiCol.x, imguiCol.y, imguiCol.z, imguiCol.w};

    stack.pushColor(ImGuiCol_Button, buttonColor);
    stack.pushColor(ImGuiCol_ButtonActive, buttonColor);
    stack.pushColor(ImGuiCol_ButtonHovered, buttonColor);
  }

  if (ImGui::Button(getTransformOperationIcon(transformOperation))) {
    editorManager.setTransformOperation(transformOperation);
  }
}

const char *TransformOperationControl::getTransformOperationIcon(
    TransformOperation transformOperation) {
  switch (transformOperation) {
  case TransformOperation::Scale:
    return fa::ExpandAlt;
  case TransformOperation::Rotate:
    return fa::Rotate;
  case TransformOperation::Move:
  default:
    return fa::Arrows;
  }
}

} // namespace liquid::editor
