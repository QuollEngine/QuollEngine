#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"
#include "quoll/editor/ui/Widgets.h"
#include "Inspector.h"

namespace quoll::editor {

Inspector::Inspector() {
  mTabs.push_back({"Entity", fa::Wrench,
                   [this](WorkspaceState &state, AssetCache &assetCache,
                          ActionExecutor &actionExecutor) {
                     mEntityPanel.renderContent(state, assetCache,
                                                actionExecutor);
                   }});
}

void Inspector::render(WorkspaceState &state, AssetCache &assetCache,
                       ActionExecutor &actionExecutor) {
  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  if (auto _ = widgets::Window("Inspector")) {
    const static auto TabItemSize = ImVec2(38, 38);

    StyleStack stack;
    stack.pushStyle(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    stack.pushStyle(ImGuiStyleVar_ChildRounding, 0.0f);

    {
      StyleStack stack;
      stack.pushColor(ImGuiCol_ChildBg,
                      Theme::getColor(ThemeColor::Charcoal800));

      auto SelectionColor = Theme::getColor(ThemeColor::Charcoal700);

      if (ImGui::BeginChild("Tabs", ImVec2(TabItemSize.x, 0), false)) {
        StyleStack stack;
        stack.pushStyle(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        stack.pushColor(ImGuiCol_HeaderHovered, SelectionColor);
        stack.pushColor(ImGuiCol_HeaderActive, SelectionColor);

        for (usize i = 0; i < mTabs.size(); ++i) {
          const auto &tab = mTabs.at(i);
          bool selected = mSelectedIndex == i;

          StyleStack tabStyles;
          if (selected) {
            tabStyles.pushColor(ImGuiCol_Header, SelectionColor);
          }

          if (ImGui::Selectable(tab.icon.c_str(), selected, 0, TabItemSize)) {
            mSelectedIndex = i;
          }
        }
      }

      ImGui::EndChild();
    }

    ImGui::SameLine();

    {
      StyleStack stack;
      stack.pushStyle(ImGuiStyleVar_WindowPadding,
                      Theme::getStyles().windowPadding);
      stack.pushColor(ImGuiCol_ChildBg,
                      Theme::getColor(ThemeColor::Charcoal700));

      if (ImGui::BeginChild("Content", ImVec2(0, 0), false,
                            ImGuiWindowFlags_AlwaysUseWindowPadding)) {

        StyleStack stack;
        stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                        Theme::getStyles().itemSpacing);
        stack.pushStyle(ImGuiStyleVar_ChildRounding,
                        Theme::getStyles().childRounding);

        mTabs.at(mSelectedIndex).renderFn(state, assetCache, actionExecutor);
      }
      ImGui::EndChild();
    }
  }
}

} // namespace quoll::editor
