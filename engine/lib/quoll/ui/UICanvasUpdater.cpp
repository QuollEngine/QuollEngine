#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/system/SystemView.h"
#include "UICanvas.h"
#include "UICanvasRenderRequest.h"
#include "UICanvasUpdater.h"

namespace quoll {

static constexpr f32 ImageSize = 50.0f;

void renderView(UIComponent component, YGNodeRef node) {
  const f32 left = YGNodeLayoutGetLeft(node);
  const f32 top = YGNodeLayoutGetTop(node);

  ImGui::SetCursorPos({left, top});

  if (auto *image = std::get_if<UIImage>(&component)) {
    if (image->texture) {
      imgui::image(image->texture, ImVec2(ImageSize, ImageSize));
    }
  } else if (auto *text = std::get_if<UIText>(&component)) {
    ImGui::Text("%s", text->content.c_str());
  } else if (auto *view = std::get_if<UIView>(&component)) {
    ImGui::PushStyleColor(
        ImGuiCol_ChildBg,
        ImVec4(view->style.backgroundColor.x, view->style.backgroundColor.y,
               view->style.backgroundColor.z, view->style.backgroundColor.w));

    const f32 width = YGNodeLayoutGetWidth(node);
    const f32 height = YGNodeLayoutGetHeight(node);

    ImGui::BeginChild(view->id.c_str(), ImVec2(width, height), false, 0);
    for (usize i = 0; i < view->children.size(); ++i) {
      const auto &childView = view->children.at(i);
      YGNodeRef childNode = YGNodeGetChild(node, static_cast<u32>(i));
      renderView(childView, childNode);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }
}

void updateLayout(UIComponent component, YGNodeRef node) {
  if (auto *image = std::get_if<UIImage>(&component)) {
    YGNodeStyleSetWidth(node, ImageSize);
    YGNodeStyleSetHeight(node, ImageSize);
  } else if (auto *text = std::get_if<UIText>(&component)) {
    auto textSize = ImGui::CalcTextSize(text->content.c_str());
    textSize.y += ImGui::GetStyle().ItemSpacing.y;
    YGNodeStyleSetWidth(node, textSize.x);
    YGNodeStyleSetHeight(node, textSize.y);
  } else if (auto *view = std::get_if<UIView>(&component)) {
    YGNodeStyleSetFlexGrow(node, view->style.grow);
    YGNodeStyleSetFlexShrink(node, view->style.shrink);
    YGNodeStyleSetFlexDirection(node, view->style.direction);
    YGNodeStyleSetAlignItems(node, view->style.alignItems);
    YGNodeStyleSetAlignContent(node, view->style.alignContent);
    YGNodeStyleSetJustifyContent(node, view->style.justifyContent);

    for (usize i = 0; i < view->children.size(); ++i) {
      YGNodeRef childNode = YGNodeNew();
      YGNodeInsertChild(node, childNode, static_cast<u32>(i));
      updateLayout(view->children.at(i), childNode);
    }
  }
}

void generateIds(UIView *component, u32 &id) {
  component->id = std::to_string(id);
  for (auto &child : component->children) {
    id++;
    if (auto *image = std::get_if<UIImage>(&child)) {
      image->id = std::to_string(id);
    } else if (auto *text = std::get_if<UIText>(&child)) {
      text->id = std::to_string(id);
    } else if (auto *view = std::get_if<UIView>(&child)) {
      generateIds(view, id);
    }
  }
}

void updateLayout(EntityDatabase &entityDatabase, const glm::vec2 &size) {
  for (auto [entity, canvas, request] :
       entityDatabase.view<UICanvas, UICanvasRenderRequest>()) {
    canvas.rootView = request.view;

    if (canvas.flexRoot) {
      YGNodeFreeRecursive(canvas.flexRoot);
      canvas.flexRoot = nullptr;
    }
    canvas.flexRoot = YGNodeNew();
    updateLayout(canvas.rootView, canvas.flexRoot);

    u32 id = 0;
    generateIds(&canvas.rootView, id);

    YGNodeCalculateLayout(canvas.flexRoot, size.x, size.y,
                          YGDirection::YGDirectionLTR);
  }

  entityDatabase.destroyComponents<UICanvasRenderRequest>();
}

void UICanvasUpdater::render(SystemView &view) {
  auto &entityDatabase = view.scene->entityDatabase;
  updateLayout(entityDatabase, mSize);

  for (auto [entity, canvas] : entityDatabase.view<UICanvas>()) {
    if (!canvas.flexRoot)
      continue;

    if (mViewportChanged) {
      YGNodeCalculateLayout(canvas.flexRoot, mSize.x, mSize.y,
                            YGDirection::YGDirectionLTR);
      mViewportChanged = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowPos(ImVec2(mPosition.x, mPosition.y));
    ImGui::SetNextWindowSize(ImVec2(mSize.x, mSize.y));

    const ImGuiWindowFlags WindowFlags =
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollWithMouse;

    if (ImGui::Begin(std::to_string(static_cast<u32>(entity)).c_str(), nullptr,
                     WindowFlags)) {
      renderView(canvas.rootView, canvas.flexRoot);
      ImGui::End();
    }

    ImGui::PopStyleVar();
  }
}

void UICanvasUpdater::setViewport(f32 x, f32 y, f32 width, f32 height) {
  static const f32 Epsilon = 0.01f;

  const bool xEqual = glm::epsilonEqual(x, mPosition.x, Epsilon);
  const bool yEqual = glm::epsilonEqual(y, mPosition.y, Epsilon);
  const bool widthEqual = glm::epsilonEqual(width, mSize.x, Epsilon);
  const bool heightEqual = glm::epsilonEqual(height, mSize.y, Epsilon);

  const bool changed = !(xEqual && yEqual && widthEqual && heightEqual);
  if (changed) {
    mViewportChanged = true;
  }

  mPosition = {x, y};
  mSize = {width, height};
}

} // namespace quoll
