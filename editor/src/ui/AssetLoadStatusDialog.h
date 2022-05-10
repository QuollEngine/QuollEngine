#pragma once

namespace liquidator {

/**
 * @brief Asset loading status dialog component
 *
 * Shows the dialog if it has warnings or error
 */
class AssetLoadStatusDialog {
  using RenderFn = std::function<void()>;

public:
  /**
   * @brief Create asset load status dialog
   *
   * @param title Dialog title
   * @param okayButton Okay button
   */
  AssetLoadStatusDialog(const liquid::String &title,
                        const liquid::String &okayButton = "Okay");

  /**
   * @brief Show dialog
   */
  void show();

  /**
   * @brief Render dialog
   */
  void render();

  /**
   * @brief Set dialog title
   *
   * @param title Title
   */
  void setTitle(const liquid::String &title);

  /**
   * @brief Set messages in dialog
   *
   * @param messages Dialog messages
   */
  void setMessages(const std::vector<liquid::String> &messages);

private:
  bool mOpen = false;

  liquid::String mTitle;
  liquid::String mOkayButton;
  std::vector<liquid::String> mMessages;
};

} // namespace liquidator
