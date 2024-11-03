#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/platform/tools/FileOpener.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll/editor/asset/AssetLoader.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "quoll/editor/ui/Widgets.h"
#include "AssetBrowser.h"

namespace quoll::editor {

static constexpr u32 ItemWidth = 90;
static constexpr u32 ItemHeight = 100;
static constexpr ImVec2 IconSize(80.0f, 80.0f);
static constexpr f32 ImagePadding = ((ItemWidth * 1.0f) - IconSize.x) / 2.0f;
static constexpr u32 TextWidth = ItemWidth - 8;

struct ImguiInputTextCallbackUserData {
  String &value;
};

namespace {

int InputTextCallback(ImGuiInputTextCallbackData *data) {
  auto *userData =
      static_cast<ImguiInputTextCallbackUserData *>(data->UserData);
  if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
    auto &str = userData->value;
    QuollAssert(data->Buf == str.c_str(),
                "Buffer and string value must point to the same address");
    str.resize(data->BufTextLen);
    data->Buf = str.data();
  }
  return 0;
}

bool ImguiInputText(const String &label, String &value,
                    ImGuiInputTextFlags flags = 0) {
  QuollAssert((flags & ImGuiInputTextFlags_CallbackResize) == 0,
              "Do not back callback resize flag");

  flags |= ImGuiInputTextFlags_CallbackResize;

  ImguiInputTextCallbackUserData userData{
      value,
  };
  return ImGui::InputText(label.c_str(), value.data(), value.capacity() + 1,
                          flags, InputTextCallback, &userData);
}

EditorIcon getIconFromAssetType(AssetType type) {
  switch (type) {
  case AssetType::Texture:
    return EditorIcon::Texture;
  case AssetType::Font:
    return EditorIcon::Font;
  case AssetType::Material:
    return EditorIcon::Material;
  case AssetType::Mesh:
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
    return EditorIcon::LuaScript;
  case AssetType::Animator:
    return EditorIcon::Animator;
  case AssetType::InputMap:
    return EditorIcon::InputMap;
  case AssetType::Environment:
    return EditorIcon::Environment;
  case AssetType::Scene:
    return EditorIcon::Scene;

  default:
    return EditorIcon::Unknown;
  }
}

} // namespace

void AssetBrowser::render(WorkspaceState &state, AssetManager &assetManager,
                          ActionExecutor &actionExecutor) {

  if (mCurrentPath.empty()) {
    setCurrentFetch(assetManager.getAssetsPath(), PathType::Directory);
  }

  if (mNeedsRefresh) {
    mEntries.clear();
    if (mCurrentPathType == PathType::Directory) {
      fetchAssetDirectory(mCurrentPath, assetManager);
    } else if (mCurrentPathType == PathType::ComplexAsset) {
      fetchAssetContents(mCurrentPath, assetManager);
    }

    mNeedsRefresh = false;
  }

  if (auto _ = widgets::Window("Asset Browser")) {
    const auto &size = ImGui::GetContentRegionAvail();
    auto itemsPerRow = static_cast<i32>(size.x / ItemWidth);

    if (itemsPerRow == 0)
      itemsPerRow = 1;

    auto relativePath =
        std::filesystem::relative(mCurrentPath, assetManager.getAssetsPath());

    if (mCurrentPath != assetManager.getAssetsPath()) {
      if (widgets::Button("Back")) {
        setCurrentFetch(mCurrentPath.parent_path(), PathType::Directory);
      }
      ImGui::SameLine();
    }

    ImGui::Text("%s", relativePath.string().c_str());

    if (ImGui::BeginTable("CurrentDir", itemsPerRow,
                          ImGuiTableFlags_NoPadInnerX)) {
      usize i = 0;

      for (; i < mEntries.size(); ++i) {
        const auto &entry = mEntries.at(i);
        auto colIndex = i % itemsPerRow;
        if (colIndex == 0) {
          ImGui::TableNextRow(ImGuiTableRowFlags_None, ItemHeight * 1.0f);
        }

        const auto &filename = entry.path.filename();

        ImGui::TableNextColumn();

        const String id = "###" + std::to_string(i);
        if (ImGui::Selectable(id.c_str(), mSelected == i,
                              ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(ItemWidth, ItemHeight))) {
          // Select when item is clicked
          mSelected = i;

          // Double click opens the file/directory
          if (ImGui::IsMouseDoubleClicked(0)) {
            if (entry.pathType == PathType::Directory) {
              setCurrentFetch(entry.path, entry.pathType);
            } else if (entry.assetType == AssetType::Prefab) {
              auto prefab =
                  assetManager.getCache().request<PrefabAsset>(entry.uuid);

              if (prefab) {
                actionExecutor.execute<SpawnPrefabAtView>(prefab.data(),
                                                          state.camera);
              } else {
                // TODO: Handle error
              }
            } else if (entry.assetType == AssetType::Texture) {
              auto texture =
                  assetManager.getCache().request<TextureAsset>(entry.uuid);
              if (texture) {
                actionExecutor.execute<SpawnSpriteAtView>(texture.data(),
                                                          state.camera);
              } else {
                // TODO: Handle error
              }

            } else if (entry.assetType == AssetType::Material) {
              auto material =
                  assetManager.getCache().request<MaterialAsset>(entry.uuid);
              if (material) {
                mMaterialViewer.open(material);
              } else {
                // TODO: Handle error
              }
            } else {
              platform::FileOpener::openFile(entry.path);
            }
          }
        }

        const bool dndAllowed = entry.assetType == AssetType::Prefab ||
                                entry.assetType == AssetType::Mesh ||
                                entry.assetType == AssetType::Material ||
                                entry.assetType == AssetType::Skeleton ||
                                entry.assetType == AssetType::Texture ||
                                entry.assetType == AssetType::Audio ||
                                entry.assetType == AssetType::LuaScript ||
                                entry.assetType == AssetType::Environment ||
                                entry.assetType == AssetType::Animator ||
                                entry.assetType == AssetType::InputMap;

        if (dndAllowed) {
          if (ImGui::BeginDragDropSource()) {
            auto uuid = entry.uuid.toString();
            ImGui::SetDragDropPayload(
                getAssetTypeString(entry.assetType).c_str(), uuid.c_str(),
                uuid.length() + 1);
            renderEntry(entry);
            ImGui::EndDragDropSource();
          }
        }

        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::Text("%s", entry.name.c_str());
          ImGui::EndTooltip();
        }

        if (entry.pathType != PathType::Directory &&
            ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
          ImGui::OpenPopup(id.c_str(), ImGuiPopupFlags_MouseButtonRight);
        }

        if (ImGui::BeginPopup(id.c_str())) {
          if (entry.pathType == PathType::ComplexAsset &&
              ImGui::MenuItem("View contents")) {
            setCurrentFetch(entry.path, entry.pathType);
          }

          if (ImGui::MenuItem("Copy UUID")) {
            ImGui::SetClipboardText(entry.uuid.toString().c_str());
          }

          ImGui::EndPopup();
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

        imgui::image(mStagingEntry.preview, IconSize);
        ImGui::PushItemWidth(ItemWidth);

        if (!mInitialFocusSet) {
          ImGui::SetKeyboardFocusHere();
          mInitialFocusSet = true;
        }

        ImguiInputText("###StagingEntryName", mStagingEntry.name);

        if (ImGui::IsItemDeactivated()) {
          handleCreateEntry(assetManager);
        }
      }
      ImGui::TableNextRow();
    }

    ImGui::EndTable();

    if (mCurrentPathType == PathType::Directory) {
      if (ImGui::BeginPopupContextWindow(
              "AssetBrowserPopup",
              ImGuiPopupFlags_NoOpenOverItems |
                  ImGuiPopupFlags_MouseButtonRight |
                  ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if (ImGui::MenuItem("Import asset")) {
          handleAssetImport(assetManager);
        }

        if (ImGui::MenuItem("Create directory")) {
          mHasStagingEntry = true;
          mStagingEntry.preview = IconRegistry::getIcon(EditorIcon::Directory);
          mStagingEntry.pathType = PathType::Directory;
          mStagingEntry.isEditable = true;
        }

        if (ImGui::MenuItem("Create Lua script")) {
          mHasStagingEntry = true;
          mStagingEntry.preview = IconRegistry::getIcon(EditorIcon::LuaScript);
          mStagingEntry.pathType = PathType::SimpleAsset;
          mStagingEntry.isEditable = true;
          mStagingEntry.assetType = AssetType::LuaScript;
        }

        if (ImGui::MenuItem("Create animator")) {
          mHasStagingEntry = true;
          mStagingEntry.preview = IconRegistry::getIcon(EditorIcon::Animator);
          mStagingEntry.pathType = PathType::SimpleAsset;
          mStagingEntry.isEditable = true;
          mStagingEntry.assetType = AssetType::Animator;
        }

        if (ImGui::MenuItem("Create input map")) {
          mHasStagingEntry = true;
          mStagingEntry.preview = IconRegistry::getIcon(EditorIcon::InputMap);
          mStagingEntry.pathType = PathType::SimpleAsset;
          mStagingEntry.isEditable = true;
          mStagingEntry.assetType = AssetType::InputMap;
        }
        ImGui::EndPopup();
      }
    }
  }

  mStatusDialog.render();

  mMaterialViewer.render();
}

void AssetBrowser::reload() { mNeedsRefresh = true; }

void AssetBrowser::handleAssetImport(AssetManager &assetManager) {
  auto res = AssetLoader(assetManager).loadFromFileDialog(mCurrentPath);

  if (!res) {
    mStatusDialog.setTitle("Import failed");
    mStatusDialog.setMessages({res.error()});
    mStatusDialog.show();

    Engine::getUserLogger().error() << res.error().message();

    return;
  }

  if (res.hasWarnings()) {
    mStatusDialog.setTitle("Import successful with warnings");
    mStatusDialog.setMessages(res.warnings());
    mStatusDialog.show();

    for (const auto &warning : res.warnings()) {
      Engine::getUserLogger().warning() << warning;
    }
  }

  reload();
}

void AssetBrowser::handleCreateEntry(AssetManager &assetManager) {
  if (!mStagingEntry.name.empty()) {
    auto path = mCurrentPath / mStagingEntry.name;
    if (mStagingEntry.pathType == PathType::Directory) {
      assetManager.createDirectory(path);
    } else if (mStagingEntry.assetType == AssetType::LuaScript) {
      assetManager.createLuaScript(path);
    } else if (mStagingEntry.assetType == AssetType::Animator) {
      assetManager.createAnimator(path);
    } else if (mStagingEntry.assetType == AssetType::InputMap) {
      assetManager.createInputMap(path);
    }
  }

  // Reset values and hide staging
  mStagingEntry.name = "";
  mStagingEntry.pathType = PathType::SimpleAsset;
  mHasStagingEntry = false;
  mInitialFocusSet = false;

  reload();
}

void AssetBrowser::renderEntry(const Entry &entry) {

  {
    const f32 initialCursorPos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(initialCursorPos + ImagePadding);
    imgui::image(entry.preview, IconSize);
    ImGui::SetCursorPosX(initialCursorPos);
  }

  {
    const f32 initialCursorPos = ImGui::GetCursorPosX();
    const f32 centerPos =
        initialCursorPos + (ItemWidth * 1.0f - entry.textWidth) * 0.5f;
    ImGui::SetCursorPosX(centerPos);
    ImGui::Text("%s", entry.truncatedName.c_str());
    ImGui::SetCursorPosX(initialCursorPos);
  }
}

void AssetBrowser::fetchAssetDirectory(Path path, AssetManager &assetManager) {
  mCurrentPath = path;

  for (auto &directoryEntry : std::filesystem::directory_iterator(path)) {
    auto filePath = directoryEntry.path();
    if (filePath.extension() == ".meta") {
      continue;
    }

    Entry entry;
    entry.path = filePath;
    entry.name = filePath.filename().string();

    if (!directoryEntry.is_directory()) {
      const auto &sourceInfo = assetManager.getSourceInfo(filePath);
      const auto &engineAssetUuid = assetManager.findRootAssetUuid(filePath);

      entry.assetType = sourceInfo.type;
      entry.uuid = sourceInfo.uuid;
      if (sourceInfo.hasContents) {
        entry.pathType = PathType::ComplexAsset;
      } else {
        entry.pathType = PathType::SimpleAsset;
      }
    } else {
      entry.pathType = PathType::Directory;
    }

    setDefaultProps(entry, assetManager);
    mEntries.push_back(entry);
  }

  std::sort(mEntries.begin(), mEntries.end(), [](const auto &a, const auto &b) {
    if ((a.pathType == PathType::Directory ||
         b.pathType == PathType::Directory) &&
        a.pathType != b.pathType) {
      return a.pathType > b.pathType;
    }

    return a.name < b.name;
  });
}

void AssetBrowser::fetchAssetContents(Path path, AssetManager &assetManager) {
  mCurrentPath = path;
  const auto &contentInfos = assetManager.getSourceContentInfos(path);

  for (const auto &info : contentInfos) {
    Entry entry;
    entry.pathType = PathType::SimpleAsset;
    entry.path = path / info.name;
    entry.assetType = info.type;
    entry.uuid = info.uuid;
    entry.name = info.name;

    setDefaultProps(entry, assetManager);
    mEntries.push_back(entry);
  }
}

void AssetBrowser::setDefaultProps(Entry &entry, AssetManager &assetManager) {
  // Name
  String ellipsis = "..";
  auto calculateTextWidth = [&ellipsis](const String &name) {
    return ImGui::CalcTextSize((name + ellipsis).c_str()).x;
  };

  entry.textWidth = calculateTextWidth(entry.name);
  entry.truncatedName = entry.name;

  if (ImGui::CalcTextSize(entry.truncatedName.c_str()).x > TextWidth) {
    while (calculateTextWidth(entry.truncatedName) > TextWidth) {
      entry.truncatedName.pop_back();
    }

    entry.textWidth = calculateTextWidth(entry.truncatedName);
    entry.truncatedName += ellipsis;
  }

  entry.preview = assetManager.generatePreview(entry.uuid);
  if (!rhi::isHandleValid(entry.preview)) {
    auto icon = entry.pathType == PathType::Directory
                    ? EditorIcon::Directory
                    : getIconFromAssetType(entry.assetType);
    entry.preview = IconRegistry::getIcon(icon);
  }
}

void AssetBrowser::setCurrentFetch(Path path, PathType pathType) {
  mCurrentPath = path;
  mCurrentPathType = pathType;
  reload();
}

} // namespace quoll::editor
