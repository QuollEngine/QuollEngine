#pragma once

namespace quoll::editor {

/**
 * @brief Confirmation dialog component
 */
class ConfirmationDialog {
public:
  /**
   * @brief Create confirmation dialog
   *
   * @param title Dialog title
   * @param prompt Confirmation prompt
   * @param confirmButtonLabel Confirm button label
   * @param cancelButtonLabel Cancel button label
   */
  ConfirmationDialog(const String &title, const String &prompt,
                     const String &confirmButtonLabel = "Confirm",
                     const String &cancelButtonLabel = "Cancel");

  /**
   * @brief Show confirmation dialog
   */
  void show();

  /**
   * @brief Render confirmation dialog
   */
  void render();

  /**
   * @brief Check if prompt is confirmed
   *
   * @retval true Prompt is confirmed
   * @retval false Prompt is not confirmed
   */
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
