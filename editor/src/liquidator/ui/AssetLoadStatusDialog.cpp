#include "liquid/core/Base.h"
#include "AssetLoadStatusDialog.h"

#include <imgui.h>

namespace liquidator {

AssetLoadStatusDialog::AssetLoadStatusDialog(const liquid::String &title,
                                             const liquid::String &okayButton)
    : mTitle(title), mOkayButton(okayButton) {}

void AssetLoadStatusDialog::show() { mOpen = true; }

void AssetLoadStatusDialog::render() {
  if (mOpen) {
    ImGui::OpenPopup(mTitle.c_str());
  }

  if (ImGui::BeginPopupModal(mTitle.c_str())) {

    for (auto &message : mMessages) {
      ImGui::BulletText("%s", message.c_str());
    }

    if (ImGui::Button(mOkayButton.c_str())) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  mOpen = false;
}

void AssetLoadStatusDialog::setTitle(const liquid::String &title) {
  mTitle = title;
}

void AssetLoadStatusDialog::setMessages(
    const std::vector<liquid::String> &messages) {
  mMessages = messages;
}

} // namespace liquidator
