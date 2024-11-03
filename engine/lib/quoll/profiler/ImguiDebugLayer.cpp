#include "quoll/core/Base.h"
#include "quoll/core/Property.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/qui/Components.h"
#include "ImguiDebugLayer.h"

namespace quoll {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static constexpr auto ColoredText =
    qui::component([](qui::Value<quoll::String> text) {
      return qui::Box(qui::Text(text).color(qui::Color::Yellow))
          .background(qui::Color::Red)
          .width(40.0f)
          .height(40.0f);
    });

static constexpr auto DemoQuiFlexbox = qui::component([]() {
  return qui::Box(qui::Flex({
                                ColoredText("1"),
                                ColoredText("2"),
                                ColoredText("3"),
                                ColoredText("4"),
                                ColoredText("5"),
                                ColoredText("6"),
                                ColoredText("7"),
                                ColoredText("8"),
                            })
                      .wrap(qui::Wrap::Wrap)
                      .spacing(glm::vec2(5.0f)))
      .width(200.0f)
      .height(300.0f);
});

struct ButtonStyle {
  qui::Color color;
  qui::Color backgroundColor;
};

static constexpr auto ColoredButton = qui::component(
    [](qui::Scope &scope, qui::Value<quoll::String> text,
       qui::Value<ButtonStyle> style, qui::Value<ButtonStyle> hoverStyle,
       qui::Value<ButtonStyle> activeStyle) {
      auto hovered = scope.signal(false);
      auto clicked = scope.signal(false);

      auto color =
          scope.computation([hovered, clicked, hoverStyle, activeStyle, style] {
            if (hovered()) {
              return hoverStyle().color;
            }

            return clicked() ? activeStyle().color : style().color;
          });

      auto backgroundColor =
          scope.computation([hovered, clicked, hoverStyle, activeStyle, style] {
            if (hovered()) {
              return hoverStyle().backgroundColor;
            }

            return clicked() ? activeStyle().backgroundColor
                             : style().backgroundColor;
          });

      return qui::Pressable(qui::Box(qui::Text(text).color(color))
                                .padding(qui::EdgeInsets(5.0f))
                                .background(backgroundColor))
          .onHoverIn([hovered](const auto &) mutable { hovered.set(true); })
          .onHoverOut([hovered](const auto &) mutable { hovered.set(false); })
          .onPress(
              [clicked](const auto &) mutable { clicked.set(!clicked()); });
    });

static constexpr auto DemoPressable = qui::component([]() {
  return ColoredButton(
      "Hello world",
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0x2e86deff},
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0x54a0ffff},
      ButtonStyle{.color = qui::Color::White, .backgroundColor = 0xee5253ff});
});

static constexpr auto DemoQui = qui::component([]() {
  return qui::Flex({qui::Box(qui::Text("Red text on a yellow background")
                                 .color(qui::Color::Red))
                        .padding(qui::EdgeInsets(5.0f))
                        .background(qui::Color::Yellow)
                        .width(100.0f)
                        .borderRadius(5.0f),

                    qui::Box(qui::Text("Text with resizable sizing"))
                        .background(qui::Color::White)
                        .padding(qui::EdgeInsets(5.0f)),

                    DemoQuiFlexbox(), DemoPressable()})
      .spacing(glm::vec2{10.0f, 0.0f});
});
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

ImguiDebugLayer::ImguiDebugLayer(
    const rhi::PhysicalDeviceInformation &physicalDeviceInfo,
    const rhi::DeviceStats &deviceStats, const FPSCounter &fpsCounter,
    MetricsCollector &metricsCollector, AssetCache *assetCache)
    : mPhysicalDeviceInfo(physicalDeviceInfo), mFpsCounter(fpsCounter),
      mMetricsCollector(metricsCollector), mDeviceStats(deviceStats),
      mAssetCache(assetCache) {
  mDemoTree = qui::Qui::createTree(DemoQui());
}

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
    ImGui::MenuItem("Qui demo", nullptr, &mQuiVisible);
    ImGui::EndMenu();
  }
}

void ImguiDebugLayer::render() {
  renderPhysicalDeviceInfo();
  renderUsageMetrics();
  renderPerformanceMetrics();
  renderAssets();
  renderDemoWindow();
  renderQui();
}

void ImguiDebugLayer::renderPerformanceMetrics() {
  static constexpr u32 OneSecondInMs = 1000;
  if (!mPerformanceMetricsVisible)
    return;

  const u32 fps = mFpsCounter.getFPS();

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

namespace {

template <typename TAssetData> void renderAssetMap(AssetCache *cache) {
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

      for (auto &[handle, asset] : map.getMetas()) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", asset.uuid.toString().c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", asset.name.c_str());

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", map.hasData(handle) ? "Yes" : "No");

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", map.getRefCount(handle));
      }

      ImGui::EndTable();
    }

    ImGui::EndTabItem();
  }
}

} // namespace

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

void ImguiDebugLayer::renderQui() {
  if (!mQuiVisible)
    return;

  if (ImGui::Begin("Qui", &mQuiVisible, ImGuiWindowFlags_NoDocking)) {
    auto size = ImGui::GetWindowSize();
    auto pos = ImGui::GetCursorScreenPos();
    qui::Qui::render(mDemoTree, {pos.x, pos.y}, {size.x, size.y});
  }
  ImGui::End();
}

} // namespace quoll
