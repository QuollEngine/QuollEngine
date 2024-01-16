#pragma once

namespace quoll::editor {

class ConfirmationDialog {
public:
  ConfirmationDialog(const String &title, const String &prompt,
                     const String &confirmButtonLabel = "Confirm",
                     const String &cancelButtonLabel = "Cancel");

  void show();

  void render();

  inline bool isConfirmed() const { return mConfirmed; }

private:
  bool mOpen = false;
  bool mConfirmed = false;

  String mTitle;
  String mPrompt;
  String mConfirmButtonLabel;
  String mCancelButtonLabel;
};

} // namespace quoll::editor
