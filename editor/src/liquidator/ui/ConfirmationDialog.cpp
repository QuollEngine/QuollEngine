#include "liquid/core/Base.h"
#include "ConfirmationDialog.h"

#include "liquid/imgui/Imgui.h"

namespace liquidator {

ConfirmationDialog::ConfirmationDialog(const liquid::String &title,
                                       const liquid::String &prompt,
                                       const liquid::String &confirmButtonLabel,
                                       const liquid::String &cancelButtonLabel)
    : mTitle(title), mPrompt(prompt), mConfirmButtonLabel(confirmButtonLabel),
      mCancelButtonLabel(cancelButtonLabel) {}

void ConfirmationDialog::show() { mOpen = true; }

void ConfirmationDialog::render() {
  if (mOpen) {
    ImGui::OpenPopup(mTitle.c_str());
  }

  if (ImGui::BeginPopupModal(mTitle.c_str())) {
    ImGui::Text("%s", mPrompt.c_str());
    if (ImGui::Button(mConfirmButtonLabel.c_str())) {
      mConfirmed = true;
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button(mCancelButtonLabel.c_str())) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  mOpen = false;
}

} // namespace liquidator
