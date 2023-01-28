#pragma once

namespace liquid::editor {

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
  AssetLoadStatusDialog(const String &title, const String &okayButton = "Okay");

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
  void setTitle(const String &title);

  /**
   * @brief Set messages in dialog
   *
   * @param messages Dialog messages
   */
  void setMessages(const std::vector<String> &messages);

private:
  bool mOpen = false;

  String mTitle;
  String mOkayButton;
  std::vector<String> mMessages;
};

} // namespace liquid::editor
