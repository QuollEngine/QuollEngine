#pragma once

#include "liquid/core/Base.h"

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class ConfirmationDialog {
  using ConfirmHandler = std::function<void(SceneManager &)>;

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
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  bool open = false;

  ConfirmHandler confirmHandler;

  liquid::String title;
  liquid::String prompt;
  liquid::String confirmButtonLabel;
  liquid::String cancelButtonLabel;
};

} // namespace liquidator
