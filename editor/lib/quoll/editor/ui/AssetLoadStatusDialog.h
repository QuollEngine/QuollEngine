#pragma once

namespace quoll::editor {

class AssetLoadStatusDialog {
  using RenderFn = std::function<void()>;

public:
  AssetLoadStatusDialog(const String &title, const String &okayButton = "Okay");

  void show();

  void render();

  void setTitle(const String &title);

  void setMessages(const std::vector<String> &messages);

private:
  bool mOpen = false;

  String mTitle;
  String mOkayButton;
  std::vector<String> mMessages;
};

} // namespace quoll::editor
