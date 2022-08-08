#pragma once

#include "liquidator/editor-scene/EditorManager.h"

namespace liquidator {

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
  ConfirmationDialog(const liquid::String &title, const liquid::String &prompt,
                     const liquid::String &confirmButtonLabel = "Confirm",
                     const liquid::String &cancelButtonLabel = "Cancel");

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

  liquid::String mTitle;
  liquid::String mPrompt;
  liquid::String mConfirmButtonLabel;
  liquid::String mCancelButtonLabel;
};

} // namespace liquidator
