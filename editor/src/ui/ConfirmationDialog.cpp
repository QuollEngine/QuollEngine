#include "liquid/core/Base.h"
#include "ConfirmationDialog.h"

#include <imgui.h>

namespace liquidator {

ConfirmationDialog::ConfirmationDialog(
    const liquid::String &title_, const liquid::String &prompt_,
    const ConfirmHandler &confirmHandler_,
    const liquid::String &confirmButtonLabel_,
    const liquid::String &cancelButtonLabel_)
    : title(title_), prompt(prompt_), confirmHandler(confirmHandler_),
      confirmButtonLabel(confirmButtonLabel_),
      cancelButtonLabel(cancelButtonLabel_) {}

void ConfirmationDialog::show() { open = true; }

void ConfirmationDialog::render(SceneManager &sceneManager) {
  if (open) {
    ImGui::OpenPopup(title.c_str());
  }

  if (ImGui::BeginPopupModal(title.c_str())) {
    ImGui::Text("%s", prompt.c_str());
    if (ImGui::Button(confirmButtonLabel.c_str())) {
      confirmHandler(sceneManager);
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button(cancelButtonLabel.c_str())) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  open = false;
}

} // namespace liquidator
