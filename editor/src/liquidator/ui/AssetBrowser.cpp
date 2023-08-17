#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/imgui/Imgui.h"
#include "liquid/imgui/ImguiUtils.h"

#include "liquid/platform/tools/FileOpener.h"

#include "liquidator/actions/SpawnEntityActions.h"

#include "Widgets.h"
#include "AssetBrowser.h"

namespace liquid::editor {

static constexpr uint32_t ItemWidth = 90;
static constexpr uint32_t ItemHeight = 100;
static constexpr ImVec2 IconSize(80.0f, 80.0f);
static constexpr float ImagePadding = ((ItemWidth * 1.0f) - IconSize.x) / 2.0f;
static constexpr uint32_t TextWidth = ItemWidth - 8;

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

void AssetBrowser::render(WorkspaceContext &context) {
  auto &assetManager = context.assetManager;

  if (mCurrentDirectory.empty()) {
    setCurrentFetch(assetManager.getAssetsPath());
  }

  if (mNeedsRefresh) {
    mEntries.clear();
    if (const auto *path = std::get_if<Path>(&mCurrentFetch)) {
      fetchAssetDirectory(*path, assetManager);
    } else if (const auto *handle =
                   std::get_if<PrefabAssetHandle>(&mCurrentFetch)) {
      fetchPrefab(*handle, assetManager);
    }

    mNeedsRefresh = false;
  }

  if (auto _ = widgets::Window("Asset Browser")) {
    const auto &size = ImGui::GetContentRegionAvail();
    auto itemsPerRow = static_cast<int32_t>(size.x / ItemWidth);

    if (itemsPerRow == 0)
      itemsPerRow = 1;

    const auto &currentFetchPath = getCurrentFetchPath();

    auto relativePath = std::filesystem::relative(currentFetchPath,
                                                  assetManager.getAssetsPath());

    if (currentFetchPath != assetManager.getAssetsPath()) {
      if (ImGui::Button("Back")) {
        setCurrentFetch(currentFetchPath.parent_path());
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
              setCurrentFetch(entry.path);
            } else if (entry.assetType == AssetType::Prefab) {
              context.actionExecutor.execute<SpawnPrefabAtView>(
                  static_cast<PrefabAssetHandle>(entry.asset),
                  context.state.camera);
            } else if (entry.assetType == AssetType::Texture) {
              context.actionExecutor.execute<SpawnSpriteAtView>(
                  static_cast<TextureAssetHandle>(entry.asset),
                  context.state.camera);
            } else if (entry.assetType == AssetType::Material) {
              mMaterialViewer.open(
                  static_cast<MaterialAssetHandle>(entry.asset));
            } else {
              platform::FileOpener::openFile(entry.path);
            }
          }
        }

        bool dndAllowed = entry.assetType == AssetType::Prefab ||
                          entry.assetType == AssetType::Mesh ||
                          entry.assetType == AssetType::SkinnedMesh ||
                          entry.assetType == AssetType::Material ||
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
          ImGui::Text("%s", entry.name.c_str());
          ImGui::EndTooltip();
        }

        if (entry.assetType == AssetType::Prefab) {
          if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(id.c_str(), ImGuiPopupFlags_MouseButtonRight);
          }

          if (ImGui::BeginPopup(id.c_str())) {
            if (ImGui::MenuItem("View contents")) {
              setCurrentFetch(static_cast<PrefabAssetHandle>(entry.asset));
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

        imgui::image(IconRegistry::getIcon(mStagingEntry.icon), IconSize);
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

    if (std::get_if<Path>(&mCurrentFetch) != nullptr) {
      if (ImGui::BeginPopupContextWindow(
              "AssetBrowserPopup",
              ImGuiPopupFlags_NoOpenOverItems |
                  ImGuiPopupFlags_MouseButtonRight |
                  ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if (ImGui::MenuItem("Import asset")) {
          handleAssetImport(context.assetManager);
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

void AssetBrowser::reload() { mNeedsRefresh = true; }

void AssetBrowser::handleAssetImport(AssetManager &assetManager) {
  auto res = AssetLoader(assetManager).loadFromFileDialog(mCurrentDirectory);

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
  if (!mStagingEntry.name.empty()) {
    auto path = mCurrentDirectory / mStagingEntry.name;
    if (mStagingEntry.isDirectory) {
      assetManager.createDirectory(path);
    } else if (mStagingEntry.assetType == AssetType::LuaScript) {
      assetManager.createLuaScript(path);
    } else if (mStagingEntry.assetType == AssetType::Animator) {
      assetManager.createAnimator(path);
    }
  }

  // Reset values and hide staging
  mStagingEntry.name = "";
  mStagingEntry.isDirectory = false;
  mHasStagingEntry = false;
  mInitialFocusSet = false;

  reload();
}

void AssetBrowser::renderEntry(const Entry &entry) {

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
    ImGui::Text("%s", entry.truncatedName.c_str());
    ImGui::SetCursorPosX(initialCursorPos);
  }
}

void AssetBrowser::fetchAssetDirectory(Path path, AssetManager &assetManager) {
  mCurrentDirectory = path;

  for (auto &directoryEntry : std::filesystem::directory_iterator(path)) {
    auto filePath = directoryEntry.path();
    if (filePath.extension() == ".meta") {
      continue;
    }

    const auto &engineAssetUuid = assetManager.findRootAssetUuid(filePath);
    const auto &pair =
        assetManager.getAssetRegistry().getAssetByUuid(engineAssetUuid);

    Entry entry;
    entry.isDirectory = directoryEntry.is_directory();
    entry.path = filePath;
    entry.name = filePath.filename().string();
    entry.assetType = pair.first;
    entry.asset = pair.second;

    setDefaultProps(entry, assetManager.getAssetRegistry());
    mEntries.push_back(entry);
  }
}

void AssetBrowser::fetchPrefab(PrefabAssetHandle handle,
                               AssetManager &assetManager) {
  auto &assetRegistry = assetManager.getAssetRegistry();
  const auto &prefab = assetRegistry.getPrefabs().getAsset(handle);
  mPrefabDirectory = mCurrentDirectory / prefab.name;

  auto prefabName = prefab.name + "/";

  auto removePrefabName = [&prefabName](String name) {
    auto index = name.find(prefabName);
    if (index == 0) {
      auto length = prefabName.length();
      return name.substr(length);
    }
    return name;
  };

  auto createPrefabEntry = [&]<typename AssetHandle, typename AssetData>(
                               AssetMap<AssetHandle, AssetData> &map,
                               AssetHandle handle) {
    const auto &asset = map.getAsset(handle);
    Entry entry;
    entry.isDirectory = false;
    entry.path = asset.path;
    entry.name = removePrefabName(asset.name);
    entry.assetType = asset.type;
    entry.asset = static_cast<uint32_t>(handle);
    setDefaultProps(entry, assetManager.getAssetRegistry());

    return entry;
  };

  auto addPrefabEntry = [&]<typename AssetHandle, typename AssetData>(
                            AssetMap<AssetHandle, AssetData> &map,
                            AssetHandle handle,
                            std::unordered_map<AssetHandle, bool> &cache) {
    if (handle != AssetHandle::Null && !cache.contains(handle)) {
      cache.insert_or_assign(handle, true);
      mEntries.push_back(createPrefabEntry(map, handle));
      return true;
    }

    return false;
  };

  std::unordered_map<TextureAssetHandle, bool> textureCache;
  auto addTextureEntryIfExists = [&](TextureAssetHandle handle) {
    addPrefabEntry(assetRegistry.getTextures(), handle, textureCache);
  };

  std::unordered_map<MaterialAssetHandle, bool> materialCache;
  auto addMaterialEntry = [&](MaterialAssetHandle handle) {
    if (addPrefabEntry(assetRegistry.getMaterials(), handle, materialCache)) {
      const auto &material = assetRegistry.getMaterials().getAsset(handle);

      addTextureEntryIfExists(material.data.baseColorTexture);
      addTextureEntryIfExists(material.data.metallicRoughnessTexture);
      addTextureEntryIfExists(material.data.normalTexture);
      addTextureEntryIfExists(material.data.occlusionTexture);
      addTextureEntryIfExists(material.data.emissiveTexture);
    }
  };

  std::unordered_map<MeshAssetHandle, bool> meshCache;
  for (const auto &ref : prefab.data.meshes) {
    addPrefabEntry(assetRegistry.getMeshes(), ref.value, meshCache);
  }

  for (const auto &renderer : prefab.data.meshRenderers) {
    for (auto material : renderer.value.materials) {
      addMaterialEntry(material);
    }
  }

  for (const auto &renderer : prefab.data.skinnedMeshRenderers) {
    for (auto material : renderer.value.materials) {
      addMaterialEntry(material);
    }
  }

  std::unordered_map<SkeletonAssetHandle, bool> skeletonCache;
  for (const auto &ref : prefab.data.skeletons) {
    addPrefabEntry(assetRegistry.getSkeletons(), ref.value, skeletonCache);
  }

  std::unordered_map<AnimatorAssetHandle, bool> animatorCache;
  for (const auto &ref : prefab.data.animators) {
    addPrefabEntry(assetRegistry.getAnimators(), ref.value, animatorCache);
  }

  std::unordered_map<AnimationAssetHandle, bool> animationCache;
  for (const auto &ref : prefab.data.animators) {
    const auto &animator = assetRegistry.getAnimators().getAsset(ref.value);
    for (const auto &state : animator.data.states) {
      addPrefabEntry(assetRegistry.getAnimations(), state.animation,
                     animationCache);
    }
  }
}

void AssetBrowser::setDefaultProps(Entry &entry, AssetRegistry &assetRegistry) {
  // Name
  String ellipsis = "..";
  auto calculateTextWidth = [&ellipsis](const String &name) {
    return ImGui::CalcTextSize((name + ellipsis).c_str()).x;
  };

  entry.textWidth = calculateTextWidth(entry.name);
  entry.truncatedName = entry.name;

  if (ImGui::CalcTextSize(entry.truncatedName.c_str()).x > TextWidth) {
    bool changed = false;

    while (calculateTextWidth(entry.truncatedName) > TextWidth) {
      entry.truncatedName.pop_back();
    }

    entry.textWidth = calculateTextWidth(entry.truncatedName);
    entry.truncatedName += ellipsis;
  }

  // Icon and preview
  entry.icon = entry.isDirectory ? EditorIcon::Directory
                                 : getIconFromAssetType(entry.assetType);

  if (entry.assetType == AssetType::Texture) {
    entry.preview = assetRegistry.getTextures()
                        .getAsset(static_cast<TextureAssetHandle>(entry.asset))
                        .data.deviceHandle;
  } else if (entry.assetType == AssetType::Environment) {
    entry.preview =
        assetRegistry.getEnvironments()
            .getAsset(static_cast<EnvironmentAssetHandle>(entry.asset))
            .preview;
  } else {
    entry.preview = IconRegistry::getIcon(entry.icon);
  }
}

void AssetBrowser::setCurrentFetch(
    std::variant<Path, PrefabAssetHandle> fetch) {
  mCurrentFetch = fetch;
  reload();
}

const Path &AssetBrowser::getCurrentFetchPath() const {
  if (std::get_if<PrefabAssetHandle>(&mCurrentFetch)) {
    return mPrefabDirectory;
  }

  return mCurrentDirectory;
}

} // namespace liquid::editor
