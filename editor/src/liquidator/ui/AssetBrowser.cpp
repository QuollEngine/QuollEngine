#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/imgui/ImguiUtils.h"
#include "AssetBrowser.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

#include "liquidator/actions/SpawnEntityActions.h"

namespace liquid::editor {

/**
 * @brief Imgui text callback user data
 */
struct ImguiInputTextCallbackUserData {
  /**
   * Passed string value ref
   */
  String &value;
};

/**
 * @brief ImGui input text resize callback
 *
 * @param data Imgui input text callback data
 */
static int InputTextCallback(ImGuiInputTextCallbackData *data) {
  auto *userData =
      static_cast<ImguiInputTextCallbackUserData *>(data->UserData);
  if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
    auto &str = userData->value;
    LIQUID_ASSERT(data->Buf == str.c_str(),
                  "Buffer and string value must point to the same address");
    str.resize(data->BufTextLen);
    data->Buf = str.data();
  }
  return 0;
}

/**
 * @brief Input text for std::string
 *
 * @param label Label
 * @param value String value
 * @param flags Input text flags
 */
static bool ImguiInputText(const String &label, String &value,
                           ImGuiInputTextFlags flags = 0) {
  LIQUID_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0,
                "Do not back callback resize flag");

  flags |= ImGuiInputTextFlags_CallbackResize;

  ImguiInputTextCallbackUserData userData{
      value,
  };
  return ImGui::InputText(label.c_str(), value.data(), value.capacity() + 1,
                          flags, InputTextCallback, &userData);
}

/**
 * @brief Get icon type from asset type
 *
 * @param type Asset type
 * @return Icon type
 */
static EditorIcon getIconFromAssetType(AssetType type) {
  switch (type) {
  case AssetType::Texture:
    return EditorIcon::Texture;
  case AssetType::Font:
    return EditorIcon::Font;
  case AssetType::Material:
    return EditorIcon::Material;
  case AssetType::Mesh:
  case AssetType::SkinnedMesh:
    return EditorIcon::Mesh;
  case AssetType::Skeleton:
    return EditorIcon::Skeleton;
  case AssetType::Animation:
    return EditorIcon::Animation;
  case AssetType::Audio:
    return EditorIcon::Audio;
  case AssetType::Prefab:
    return EditorIcon::Prefab;
  case AssetType::LuaScript:
    return EditorIcon::Script;
  case AssetType::Animator:
    return EditorIcon::Animator;
  case AssetType::Environment:
    return EditorIcon::Environment;

  default:
    return EditorIcon::Unknown;
  }
}

AssetBrowser::AssetBrowser(AssetLoader &assetLoader)
    : mAssetLoader(assetLoader), mStatusDialog("AssetLoadStatus") {}

void AssetBrowser::render(AssetManager &assetManager,
                          IconRegistry &iconRegistry, WorkspaceState &state,
                          ActionExecutor &actionExecutor) {
  static constexpr uint32_t ItemWidth = 90;
  static constexpr uint32_t ItemHeight = 100;
  static constexpr ImVec2 IconSize(80.0f, 80.0f);
  static constexpr float ImagePadding =
      ((ItemWidth * 1.0f) - IconSize.x) / 2.0f;
  static constexpr uint32_t TextWidth = ItemWidth - 8;

  if (mDirectoryChanged) {
    if (mContentsDirectory.empty()) {
      mContentsDirectory = assetManager.getAssetsPath();
      mAssetDirectory = assetManager.getAssetsPath();
    }
    mEntries.clear();
    for (auto &dirEntry :
         std::filesystem::directory_iterator(mContentsDirectory)) {

      bool isEngineDirectory = mContentsDirectory != mAssetDirectory;

      if (isEngineDirectory & dirEntry.is_directory()) {
        continue;
      }

      Entry entry;
      entry.path = dirEntry.path();
      const auto &engineAssetPath =
          isEngineDirectory ? entry.path
                            : assetManager.findEngineAssetPath(entry.path);
      const auto &pair =
          assetManager.getAssetRegistry().getAssetByPath(engineAssetPath);
      entry.isDirectory = dirEntry.is_directory();
      entry.assetType = pair.first;
      entry.asset = pair.second;

      if (isEngineDirectory && entry.assetType == AssetType::Prefab) {
        continue;
      }

      entry.icon = entry.isDirectory ? EditorIcon::Directory
                                     : getIconFromAssetType(entry.assetType);

      entry.preview = iconRegistry.getIcon(entry.icon);

      if (entry.assetType == AssetType::Texture) {
        entry.preview =
            assetManager.getAssetRegistry()
                .getTextures()
                .getAsset(static_cast<TextureAssetHandle>(pair.second))
                .data.deviceHandle;
      } else if (entry.assetType == AssetType::Environment) {
        entry.preview =
            assetManager.getAssetRegistry()
                .getEnvironments()
                .getAsset(static_cast<EnvironmentAssetHandle>(pair.second))
                .preview;
      }

      entry.clippedName = entry.path.filename().stem().string();

      String ellipsis = "..";
      auto calculateTextWidth = [&ellipsis](Entry &entry) {
        return ImGui::CalcTextSize((entry.clippedName + ellipsis).c_str()).x;
      };

      entry.textWidth = calculateTextWidth(entry);

      if (ImGui::CalcTextSize(entry.clippedName.c_str()).x > TextWidth) {
        bool changed = false;

        while (calculateTextWidth(entry) > TextWidth) {
          entry.clippedName.pop_back();
        }

        entry.textWidth = calculateTextWidth(entry);
        entry.clippedName += ellipsis;
      }

      mEntries.push_back(entry);
    }

    mDirectoryChanged = false;
  }

  if (auto _ = widgets::Window("Asset Browser")) {
    const auto &size = ImGui::GetContentRegionAvail();
    auto itemsPerRow = static_cast<int32_t>(size.x / ItemWidth);

    if (itemsPerRow == 0)
      itemsPerRow = 1;

    auto relativePath = std::filesystem::relative(mAssetDirectory,
                                                  assetManager.getAssetsPath());

    if (mAssetDirectory != assetManager.getAssetsPath()) {
      if (ImGui::Button("Back")) {
        mAssetDirectory = mAssetDirectory.parent_path();
        mContentsDirectory = mAssetDirectory;
        mDirectoryChanged = true;
      }
      ImGui::SameLine();
    }

    ImGui::Text("%s", relativePath.string().c_str());

    bool itemContextMenu = false;

    std::optional<Entry> rightClickedEntry;

    if (ImGui::BeginTable("CurrentDir", itemsPerRow,
                          ImGuiTableFlags_NoPadInnerX)) {
      size_t i = 0;

      for (; i < mEntries.size(); ++i) {
        const auto &entry = mEntries.at(i);
        auto colIndex = i % itemsPerRow;
        if (colIndex == 0) {
          ImGui::TableNextRow(ImGuiTableRowFlags_None, ItemHeight * 1.0f);
        }

        const auto &filename = entry.path.filename();

        ImGui::TableNextColumn();

        String id = "###" + std::to_string(entry.asset) + "-" +
                    std::to_string(static_cast<uint32_t>(entry.assetType));
        if (ImGui::Selectable(id.c_str(), mSelected == i,
                              ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(ItemWidth, ItemHeight))) {
          // Select when item is clicked
          mSelected = i;

          // Double click opens the file/directory
          if (ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) {
              mAssetDirectory = entry.path;
              mContentsDirectory = entry.path;
              mDirectoryChanged = true;
            } else if (entry.assetType == AssetType::Prefab) {
              actionExecutor.execute<SpawnPrefabAtView>(
                  static_cast<PrefabAssetHandle>(entry.asset), state.camera);
            } else if (entry.assetType == AssetType::Texture) {
              actionExecutor.execute<SpawnSpriteAtView>(
                  static_cast<TextureAssetHandle>(entry.asset), state.camera);
            } else if (entry.assetType == AssetType::Material) {
              mMaterialViewer.open(
                  static_cast<MaterialAssetHandle>(entry.asset));
            } else {
              mFileOpener.openFile(entry.path);
            }
          }
        }

        bool dndAllowed = entry.assetType == AssetType::Prefab ||
                          entry.assetType == AssetType::Mesh ||
                          entry.assetType == AssetType::SkinnedMesh ||
                          entry.assetType == AssetType::Skeleton ||
                          entry.assetType == AssetType::Texture ||
                          entry.assetType == AssetType::Audio ||
                          entry.assetType == AssetType::LuaScript ||
                          entry.assetType == AssetType::Environment ||
                          entry.assetType == AssetType::Animator;

        if (dndAllowed) {
          if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(
                getAssetTypeString(entry.assetType).c_str(), &entry.asset,
                sizeof(uint32_t));
            renderEntry(entry);
            ImGui::EndDragDropSource();
          }
        }

        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::Text("%s", entry.path.filename().string().c_str());
          ImGui::EndTooltip();
        }

        if (entry.assetType == AssetType::Prefab) {
          if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(id.c_str(), ImGuiPopupFlags_MouseButtonRight);
          }

          if (ImGui::BeginPopup(id.c_str())) {
            if (ImGui::MenuItem("View contents")) {
              auto relativePath = std::filesystem::relative(
                  entry.path, assetManager.getAssetsPath());

              mContentsDirectory = assetManager.getCachePath() / relativePath;
              mAssetDirectory = entry.path;
              mDirectoryChanged = true;
            }
            ImGui::EndPopup();
          }
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ItemHeight);

        renderEntry(entry);
      }

      if (mHasStagingEntry) {
        auto colIndex = i % itemsPerRow;
        if (colIndex == 0) {
          ImGui::TableNextRow(ImGuiTableRowFlags_None, ItemHeight * 1.0f);
        }

        ImGui::TableNextColumn();

        imgui::image(iconRegistry.getIcon(mStagingEntry.icon), IconSize);
        ImGui::PushItemWidth(ItemWidth);

        if (!mInitialFocusSet) {
          ImGui::SetKeyboardFocusHere();
          mInitialFocusSet = true;
        }

        ImguiInputText("###StagingEntryName", mStagingEntry.clippedName);

        if (ImGui::IsItemDeactivated()) {
          handleCreateEntry(assetManager);
        }
      }
      ImGui::TableNextRow();
    }

    ImGui::EndTable();

    // Show context menu if not inside prefab
    if (mContentsDirectory == mAssetDirectory) {
      if (ImGui::BeginPopupContextWindow(
              "AssetBrowserPopup",
              ImGuiPopupFlags_NoOpenOverItems |
                  ImGuiPopupFlags_MouseButtonRight |
                  ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if (ImGui::MenuItem("Import asset")) {
          handleAssetImport();
        }

        if (ImGui::MenuItem("Create directory")) {
          mHasStagingEntry = true;
          mStagingEntry.icon = EditorIcon::Directory;
          mStagingEntry.isDirectory = true;
          mStagingEntry.isEditable = true;
        }

        if (ImGui::MenuItem("Create Lua script")) {
          mHasStagingEntry = true;
          mStagingEntry.icon = EditorIcon::Script;
          mStagingEntry.isDirectory = false;
          mStagingEntry.isEditable = true;
          mStagingEntry.assetType = AssetType::LuaScript;
        }

        if (ImGui::MenuItem("Create animator")) {
          mHasStagingEntry = true;
          mStagingEntry.icon = EditorIcon::Animator;
          mStagingEntry.isDirectory = false;
          mStagingEntry.isEditable = true;
          mStagingEntry.assetType = AssetType::Animator;
        }
        ImGui::EndPopup();
      }
    }
  }

  mStatusDialog.render();

  mMaterialViewer.render(assetManager.getAssetRegistry());
}

void AssetBrowser::reload() { mDirectoryChanged = true; }

void AssetBrowser::handleAssetImport() {
  auto res = mAssetLoader.loadFromFileDialog(mAssetDirectory);

  if (res.hasError()) {
    mStatusDialog.setTitle("Import failed");
    mStatusDialog.setMessages({res.getError()});
    mStatusDialog.show();

    Engine::getUserLogger().error() << res.getError();

    return;
  }

  if (res.hasWarnings()) {
    mStatusDialog.setTitle("Import successful with warnings");
    mStatusDialog.setMessages(res.getWarnings());
    mStatusDialog.show();

    for (const auto &warning : res.getWarnings()) {
      Engine::getUserLogger().warning() << warning;
    }
  }

  reload();
}

void AssetBrowser::handleCreateEntry(AssetManager &assetManager) {
  if (!mStagingEntry.clippedName.empty()) {
    auto path = mAssetDirectory / mStagingEntry.clippedName;
    if (mStagingEntry.isDirectory) {
      assetManager.createDirectory(path);
    } else if (mStagingEntry.assetType == AssetType::LuaScript) {
      assetManager.createLuaScript(path);
    } else if (mStagingEntry.assetType == AssetType::Animator) {
      assetManager.createAnimator(path);
    }
  }

  // Reset values and hide staging
  mStagingEntry.clippedName = "";
  mStagingEntry.isDirectory = false;
  mHasStagingEntry = false;
  mInitialFocusSet = false;

  // Trigger directory refresh
  mDirectoryChanged = true;
}

void AssetBrowser::renderEntry(const Entry &entry) {
  static constexpr uint32_t ItemWidth = 90;
  static constexpr uint32_t ItemHeight = 100;
  static constexpr ImVec2 IconSize(80.0f, 80.0f);
  static constexpr float ImagePadding =
      ((ItemWidth * 1.0f) - IconSize.x) / 2.0f;
  static constexpr uint32_t TextWidth = ItemWidth - 8;

  {
    float initialCursorPos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(initialCursorPos + ImagePadding);
    imgui::image(entry.preview, IconSize);
    ImGui::SetCursorPosX(initialCursorPos);
  }

  {
    float initialCursorPos = ImGui::GetCursorPosX();
    const float centerPos =
        initialCursorPos + (ItemWidth * 1.0f - entry.textWidth) * 0.5f;
    ImGui::SetCursorPosX(centerPos);
    ImGui::Text("%s", entry.clippedName.c_str());
    ImGui::SetCursorPosX(initialCursorPos);
  }
}

} // namespace liquid::editor
