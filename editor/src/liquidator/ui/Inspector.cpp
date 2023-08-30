#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "Inspector.h"
#include "Widgets.h"
#include "StyleStack.h"
#include "Theme.h"
#include "FontAwesome.h"

namespace quoll::editor {

Inspector::Inspector() {
  mTabs.push_back({"Entity", fa::Wrench,
                   [this](WorkspaceState &state, AssetRegistry &assetRegistry,
                          ActionExecutor &actionExecutor) {
                     mEntityPanel.renderContent(state, assetRegistry,
                                                actionExecutor);
                   }});
}

void Inspector::render(WorkspaceState &state, AssetRegistry &assetRegistry,
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
                      Theme::getColor(ThemeColor::Neutral100));

      auto SelectionColor = Theme::getColor(ThemeColor::Neutral300);

      if (ImGui::BeginChild("Tabs", ImVec2(TabItemSize.x, 0), false)) {
        StyleStack stack;
        stack.pushStyle(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        stack.pushColor(ImGuiCol_HeaderHovered, SelectionColor);
        stack.pushColor(ImGuiCol_HeaderActive, SelectionColor);

        for (size_t i = 0; i < mTabs.size(); ++i) {
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
                      Theme::getColor(ThemeColor::Neutral300));

      if (ImGui::BeginChild("Content", ImVec2(0, 0), false,
                            ImGuiWindowFlags_AlwaysUseWindowPadding)) {

        StyleStack stack;
        stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                        Theme::getStyles().itemSpacing);
        stack.pushStyle(ImGuiStyleVar_ChildRounding,
                        Theme::getStyles().childRounding);

        mTabs.at(mSelectedIndex).renderFn(state, assetRegistry, actionExecutor);
      }
      ImGui::EndChild();
    }
  }
}

} // namespace quoll::editor
