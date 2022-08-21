#include "liquid/core/Base.h"
#include "liquid/imgui/ImguiUtils.h"
#include "AssetBrowser.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquidator {

/**
 * @brief Imgui text callback user data
 */
struct ImguiInputTextCallbackUserData {
  /**
   * Passed string value ref
   */
  liquid::String &value;
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
static bool ImguiInputText(const liquid::String &label, liquid::String &value,
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
static EditorIcon getIconFromAssetType(liquid::AssetType type) {
  switch (type) {
  case liquid::AssetType::Texture:
    return EditorIcon::Texture;
  case liquid::AssetType::Font:
    return EditorIcon::Font;
  case liquid::AssetType::Material:
    return EditorIcon::Material;
  case liquid::AssetType::Mesh:
  case liquid::AssetType::SkinnedMesh:
    return EditorIcon::Mesh;
  case liquid::AssetType::Skeleton:
    return EditorIcon::Skeleton;
  case liquid::AssetType::Animation:
    return EditorIcon::Animation;
  case liquid::AssetType::Audio:
    return EditorIcon::Audio;
  case liquid::AssetType::Prefab:
    return EditorIcon::Prefab;
  case liquid::AssetType::LuaScript:
    return EditorIcon::Script;

  default:
    return EditorIcon::Unknown;
  }
}

AssetBrowser::AssetBrowser(AssetLoader &assetLoader)
    : mAssetLoader(assetLoader), mStatusDialog("AssetLoadStatus") {}

void AssetBrowser::render(liquid::AssetManager &assetManager,
                          IconRegistry &iconRegistry,
                          EditorManager &editorManager,
                          EntityManager &entityManager) {
  static constexpr uint32_t ItemWidth = 90;
  static constexpr uint32_t ItemHeight = 100;
  static constexpr ImVec2 IconSize(80.0f, 80.0f);
  static constexpr float ImagePadding =
      ((ItemWidth * 1.0f) - IconSize.x) / 2.0f;
  static constexpr uint32_t TextWidth = ItemWidth - 8;

  if (mDirectoryChanged) {
    if (mCurrentDirectory.empty()) {
      mCurrentDirectory = assetManager.getAssetsPath();
    }
    mEntries.clear();
    for (auto &dirEntry :
         std::filesystem::directory_iterator(mCurrentDirectory)) {
      Entry entry;
      entry.path = dirEntry.path();
      const auto &pair = assetManager.getRegistry().getAssetByPath(entry.path);
      entry.isDirectory = dirEntry.is_directory();
      entry.assetType = pair.first;
      entry.asset = pair.second;

      entry.icon = entry.isDirectory ? EditorIcon::Directory
                                     : getIconFromAssetType(entry.assetType);

      entry.preview = iconRegistry.getIcon(entry.icon);

      if (entry.assetType == liquid::AssetType::Texture) {
        auto handle =
            assetManager.getRegistry()
                .getTextures()
                .getAsset(static_cast<liquid::TextureAssetHandle>(pair.second))
                .data.deviceHandle;

        if (handle != liquid::rhi::TextureHandle::Invalid) {
          entry.preview = handle;
        }
      }

      entry.clippedName = entry.path.filename().stem().string();

      liquid::String ellipsis = "..";
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
    if (auto _ = widgets::ContextMenu()) {
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
        mStagingEntry.assetType = liquid::AssetType::LuaScript;
      }
    }

    const auto &size = ImGui::GetContentRegionAvail();
    auto itemsPerRow = static_cast<int32_t>(size.x / ItemWidth);

    if (itemsPerRow == 0)
      itemsPerRow = 1;

    auto relativePath = std::filesystem::relative(mCurrentDirectory,
                                                  assetManager.getAssetsPath());

    if (mCurrentDirectory != assetManager.getAssetsPath()) {
      if (ImGui::Button("Back")) {
        mCurrentDirectory = mCurrentDirectory.parent_path();
        mDirectoryChanged = true;
      }
      ImGui::SameLine();
    }
    ImGui::Text("%s", relativePath.string().c_str());

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

        liquid::String id =
            "###" + std::to_string(entry.asset) + "-" +
            std::to_string(static_cast<uint32_t>(entry.assetType));
        if (ImGui::Selectable(id.c_str(), mSelected == i,
                              ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(ItemWidth, ItemHeight))) {
          // Select when item is clicked
          mSelected = i;

          // Double click opens the file/directory
          if (ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) {
              mCurrentDirectory = entry.path;
              mDirectoryChanged = true;
            } else if (entry.assetType == liquid::AssetType::Prefab) {
              entityManager.spawnEntity(editorManager.getEditorCamera(),
                                        liquid::EntityNull, entry.asset,
                                        entry.assetType);
            } else if (entry.assetType == liquid::AssetType::Material) {
              mMaterialViewer.open(
                  static_cast<liquid::MaterialAssetHandle>(entry.asset));
            } else if (entry.assetType == liquid::AssetType::LuaScript) {
              mFileOpener.openFile(entry.path);
            }
          }
        }

        bool dndAllowed = entry.assetType == liquid::AssetType::Mesh ||
                          entry.assetType == liquid::AssetType::SkinnedMesh ||
                          entry.assetType == liquid::AssetType::Skeleton ||
                          entry.assetType == liquid::AssetType::Texture ||
                          entry.assetType == liquid::AssetType::Audio ||
                          entry.assetType == liquid::AssetType::LuaScript;

        if (dndAllowed) {
          if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(
                liquid::getAssetTypeString(entry.assetType).c_str(),
                &entry.asset, sizeof(uint32_t));
            renderEntry(entry);
            ImGui::EndDragDropSource();
          }
        }

        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::Text("%s", entry.path.filename().string().c_str());
          ImGui::EndTooltip();
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

        liquid::imgui::image(iconRegistry.getIcon(mStagingEntry.icon),
                             IconSize);
        ImGui::PushItemWidth(ItemWidth);

        if (!mInitialFocusSet) {
          ImGui::SetKeyboardFocusHere();
          mInitialFocusSet = true;
        }

        ImguiInputText("###StagingEntryName", mStagingEntry.clippedName);

        if (ImGui::IsItemDeactivated()) {
          handleCreateEntry();
        }
      }
      ImGui::TableNextRow();
    }
    ImGui::EndTable();
  }

  mStatusDialog.render();

  mMaterialViewer.render(assetManager.getRegistry());
}

void AssetBrowser::reload() { mDirectoryChanged = true; }

void AssetBrowser::setOnCreateEntry(std::function<void(liquid::Path)> handler) {
  mOnCreateEntry = handler;
}

void AssetBrowser::handleAssetImport() {
  auto res = mAssetLoader.loadFromFileDialog(mCurrentDirectory);

  if (res.hasError()) {
    mStatusDialog.setTitle("Import failed");
    mStatusDialog.setMessages({res.getError()});
    mStatusDialog.show();

    return;
  }

  if (res.hasWarnings()) {
    mStatusDialog.setTitle("Import successful with warnings");
    mStatusDialog.setMessages(res.getWarnings());
    mStatusDialog.show();
  }

  reload();
}

void AssetBrowser::handleCreateEntry() {
  if (!mStagingEntry.clippedName.empty()) {
    auto path = mCurrentDirectory / mStagingEntry.clippedName;
    if (mStagingEntry.isDirectory) {
      std::filesystem::create_directory(path);
    } else if (mStagingEntry.assetType == liquid::AssetType::LuaScript) {
      path.replace_extension("lua");
      std::ofstream stream(path);
      stream.close();
      mOnCreateEntry(path);
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
    liquid::imgui::image(entry.preview, IconSize);
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

} // namespace liquidator
