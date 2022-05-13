#pragma once

#include "../editor-scene/EditorManager.h"

namespace liquidator {

/**
 * @brief Confirmation dialog component
 */
class ConfirmationDialog {
  using ConfirmHandler = std::function<void(EditorManager &)>;

public:
  /**
   * @brief Create confirmation dialog
   *
   * @param title Dialog title
   * @param prompt Confirmation prompt
   * @param confirmHandler Confirm handler
   * @param confirmButtonLabel Confirm button label
   * @param cancelButtonLabel Cancel button label
   */
  ConfirmationDialog(const liquid::String &title, const liquid::String &prompt,
                     const ConfirmHandler &confirmHandler,
                     const liquid::String &confirmButtonLabel = "Confirm",
                     const liquid::String &cancelButtonLabel = "Cancel");

  /**
   * @brief Show confirmation dialog
   */
  void show();

  /**
   * @brief Render confirmation dialog
   *
   * @param editorManager Editor manager
   */
  void render(EditorManager &editorManager);

private:
  bool mOpen = false;

  ConfirmHandler mConfirmHandler;

  liquid::String mTitle;
  liquid::String mPrompt;
  liquid::String mConfirmButtonLabel;
  liquid::String mCancelButtonLabel;
};

} // namespace liquidator
