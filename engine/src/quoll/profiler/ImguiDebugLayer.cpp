#include "quoll/core/Base.h"
#include "quoll/core/Property.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/imgui/Imgui.h"
#include "ImguiDebugLayer.h"

namespace quoll {

ImguiDebugLayer::ImguiDebugLayer(
    const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
    const rhi::DeviceStats &deviceStats, const FPSCounter &fpsCounter,
    MetricsCollector &metricsCollector, AssetCache *assetCache)
    : mPhysicalDeviceInfo(physicalDeviceInfo), mFpsCounter(fpsCounter),
      mMetricsCollector(metricsCollector), mDeviceStats(deviceStats),
      mAssetCache(assetCache) {}

void ImguiDebugLayer::renderMenu() {
  if (ImGui::BeginMenu("Debug")) {
    ImGui::MenuItem("Physical Device Information", nullptr,
                    &mPhysicalDeviceInfoVisible);
    ImGui::MenuItem("Usage Metrics", nullptr, &mUsageMetricsVisible);

    ImGui::MenuItem("Performance Metrics", nullptr,
                    &mPerformanceMetricsVisible);
    if (mAssetCache) {
      ImGui::MenuItem("Assets", nullptr, &mAssetsVisible);
    }
    ImGui::MenuItem("Imgui demo", nullptr, &mDemoWindowVisible);
    ImGui::EndMenu();
  }
}

void ImguiDebugLayer::render() {
  renderPhysicalDeviceInfo();
  renderUsageMetrics();
  renderPerformanceMetrics();
  renderAssets();
  renderDemoWindow();
}

void ImguiDebugLayer::renderPerformanceMetrics() {
  static constexpr u32 OneSecondInMs = 1000;
  if (!mPerformanceMetricsVisible)
    return;

  u32 fps = mFpsCounter.getFPS();

  if (ImGui::Begin("Performance Metrics", &mPerformanceMetricsVisible,
                   ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Overview", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {

      renderTableRow("FPS", std::to_string(fps));
      renderTableRow("Frame time",
                     std::to_string(fps > 0 ? OneSecondInMs / fps : 0) + "ms");
      ImGui::EndTable();
    }

    if (ImGui::BeginTable("GPU Timings", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {
      const auto period =
          mPhysicalDeviceInfo.getLimits().timestampPeriod / 1000000.0f;

      for (const auto &metric : mMetricsCollector.measure(period)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", metric.label.c_str());
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f ms", metric.value);
      }

      ImGui::EndTable();
    }
    ImGui::End();
  }
}

void ImguiDebugLayer::renderUsageMetrics() {
  if (!mUsageMetricsVisible)
    return;

  if (ImGui::Begin("Usage Metrics", &mUsageMetricsVisible,
                   ImGuiWindowFlags_NoDocking)) {

    static const std::array<String, 3> Units{"bytes", "Kb", "Mb"};
    static constexpr f32 Kilo = 1024.0f;

    auto getSizeString = [](usize size) {
      if (size < static_cast<usize>(Kilo)) {
        return std::to_string(size) + " " + Units.at(0);
      }
      f32 humanReadableSize = static_cast<f32>(size);

      usize i = 1;
      for (; i < Units.size() && humanReadableSize >= Kilo; ++i) {
        humanReadableSize /= Kilo;
      }

      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << humanReadableSize << " "
         << Units.at(i - 1) << " (" << size << " " << Units.at(0) << ")";
      return ss.str();
    };

    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {
      // Buffers
      renderTableRow(
          "Number of buffers",
          std::to_string(mDeviceStats.getResourceMetrics()->getBuffersCount()));

      renderTableRow(
          "Total size of allocated buffers",
          getSizeString(
              mDeviceStats.getResourceMetrics()->getTotalBufferSize()));

      // Textures
      renderTableRow(
          "Number of textures",
          std::to_string(
              mDeviceStats.getResourceMetrics()->getTexturesCount()));

      // Draw calls
      renderTableRow("Number of draw calls",
                     std::to_string(mDeviceStats.getDrawCallsCount()));
      renderTableRow("Number of drawn primitives",
                     std::to_string(mDeviceStats.getDrawnPrimitivesCount()));
      renderTableRow("Number of command calls",
                     std::to_string(mDeviceStats.getCommandCallsCount()));
      renderTableRow(
          "Number of descriptors",
          std::to_string(
              mDeviceStats.getResourceMetrics()->getDescriptorsCount()));

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

void ImguiDebugLayer::renderPhysicalDeviceInfo() {
  if (!mPhysicalDeviceInfoVisible)
    return;

  if (ImGui::Begin(("Device Info: " + mPhysicalDeviceInfo.getName()).c_str(),
                   &mPhysicalDeviceInfoVisible, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTable("Table", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame)) {
      renderTableRow("Name", mPhysicalDeviceInfo.getName());
      renderTableRow("Type", mPhysicalDeviceInfo.getTypeString());

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

template <typename TAssetData> static void renderAssetMap(AssetCache *cache) {
  auto type = cache->getAssetType<TAssetData>();

  if (ImGui::BeginTabItem(getAssetTypeString(type).c_str())) {
    const auto &map = cache->getRegistry().getMap<TAssetData>();

    if (ImGui::BeginTable("Table", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_SizingStretchSame |
                              ImGuiTableFlags_RowBg)) {

      // Header
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("UUID");

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Name");

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Has loaded");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Ref count");
      }

      for (auto &[handle, asset] : map.getAssets()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", asset.uuid.toString().c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", asset.name.c_str());

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("Yes");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", map.getRefCount(handle));
      }

      ImGui::EndTable();
    }

    ImGui::EndTabItem();
  }
}

void ImguiDebugLayer::renderAssets() {
  if (!mAssetsVisible || !mAssetCache)
    return;

  if (ImGui::Begin("Assets", &mAssetsVisible, ImGuiWindowFlags_NoDocking)) {
    if (ImGui::BeginTabBar("Assets")) {
      renderAssetMap<TextureAsset>(mAssetCache);
      renderAssetMap<MaterialAsset>(mAssetCache);
      renderAssetMap<MeshAsset>(mAssetCache);
      renderAssetMap<SkeletonAsset>(mAssetCache);
      renderAssetMap<AnimationAsset>(mAssetCache);
      renderAssetMap<AnimatorAsset>(mAssetCache);
      renderAssetMap<AudioAsset>(mAssetCache);
      renderAssetMap<LuaScriptAsset>(mAssetCache);
      renderAssetMap<InputMapAsset>(mAssetCache);
      renderAssetMap<FontAsset>(mAssetCache);
      renderAssetMap<EnvironmentAsset>(mAssetCache);
      renderAssetMap<PrefabAsset>(mAssetCache);
      renderAssetMap<SceneAsset>(mAssetCache);

      ImGui::EndTabBar();
    }

    ImGui::End();
  }
}

void ImguiDebugLayer::renderDemoWindow() {
  if (!mDemoWindowVisible)
    return;

  ImGui::ShowDemoWindow(&mDemoWindowVisible);
}

void ImguiDebugLayer::renderTableRow(StringView header, StringView value) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%s", String(header).c_str());
  ImGui::TableSetColumnIndex(1);
  ImGui::Text("%s", String(value).c_str());
}

} // namespace quoll
