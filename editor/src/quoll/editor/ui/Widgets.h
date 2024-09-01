#pragma once

#include "quoll/imgui/ImguiUtils.h"

namespace quoll::editor::widgets {

bool Button(const char *label, ImVec2 size = ImVec2{0.0f, 0.0f});

/**
 * @brief Section widget
 *
 * Represents a section within a window
 * with title and contents
 *
 * Automatically applies styles based on
 * the theme values
 */
class Section : NoCopyMove {
public:
  Section(const char *title);

  ~Section();

  inline operator bool() const { return mExpanded; }

  inline const ImRect &getClipRect() const { return mClipRect; }

  inline const ImVec2 &getPadding() const { return mPadding; }

private:
  bool mExpanded = false;
  ImRect mClipRect;
  ImVec2 mPadding;
};

class Window : NoCopyMove {
public:
  Window(const char *title);

  ~Window();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

class FixedWindow : NoCopyMove {
public:
  FixedWindow(const char *title, bool &open);

  ~FixedWindow();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

class ContextMenu : NoCopyMove {
public:
  ContextMenu();

  ~ContextMenu();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

class Table : NoCopyMove {
public:
  Table(const char *id, u32 numColumns);

  ~Table();

  inline operator bool() const { return mExpanded; }

  void column(const glm::vec4 &value);

  void column(const glm::vec3 &value);

  void column(const glm::vec2 &value);

  void column(const glm::quat &value);

  void column(f32 value);

  void column(const String &value);

  void column(i32 value);

  void column(u32 value);

  void column(rhi::TextureHandle handle, const glm::vec2 &size);

  void column(const AssetRef<TextureAsset> &asset, const glm::vec2 &size);

  template <class... TColumns> void row(const TColumns &...columns) {
    ImGui::TableNextRow();

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    (column(columns), ...);
  }

private:
  bool mExpanded = false;
};

class Input {
public:
  Input(String label, glm::vec3 &value, bool autoChange = true);

  Input(String label, glm::vec2 &value, bool autoChange = true);

  Input(String label, f32 &value, bool autoChange = true);

  Input(String label, u32 &value, bool autoChange = true);

  Input(String label, String &value, bool autoChange = true);

  inline operator bool() const { return mChanged; }

private:
  void renderScalarInput(String label, void *data, usize size,
                         ImGuiDataType dataType);

  void renderTextInput(String label, String &data);

private:
  bool mChanged = false;
};

class InputColor {
public:
  InputColor(String label, glm::vec4 &value);

  inline operator bool() const { return mChanged; }

private:
  bool mChanged = false;
};

} // namespace quoll::editor::widgets
