#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"

#include "AssetLoadStatusDialog.h"
#include "Widgets.h"

namespace quoll::editor {

AssetLoadStatusDialog::AssetLoadStatusDialog(const String &title,
                                             const String &okayButton)
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

    if (widgets::Button(mOkayButton.c_str())) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  mOpen = false;
}

void AssetLoadStatusDialog::setTitle(const String &title) { mTitle = title; }

void AssetLoadStatusDialog::setMessages(const std::vector<String> &messages) {
  mMessages = messages;
}

} // namespace quoll::editor
