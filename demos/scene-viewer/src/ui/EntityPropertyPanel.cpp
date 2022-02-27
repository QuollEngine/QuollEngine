#include "liquid/core/Base.h"

#include "EntityPropertyPanel.h"
#include <imgui.h>

using liquid::Entity;
using liquid::EntityContext;
using liquid::LightComponent;
using liquid::MeshComponent;
using liquid::String;
using liquid::TransformComponent;

EntityPropertyPanel::EntityPropertyPanel(EntityContext &context_)
    : context(context_) {}

void EntityPropertyPanel::setEntity(Entity entity_) { entity = entity_; }

void EntityPropertyPanel::render() {
  if (!context.hasEntity(entity))
    return;

  String name;
  if (context.hasComponent<liquid::NameComponent>(entity)) {
    name = context.getComponent<liquid::NameComponent>(entity).name;
  } else {
    name = "Entity " + std::to_string(entity);
  }

  ImGui::Begin(name.c_str(), &panelOpen);

  if (context.hasComponent<TransformComponent>(entity)) {
    renderTransformDetails();
  }

  if (context.hasComponent<LightComponent>(entity)) {
    renderLightDetails();
  }

  ImGui::End();
}

void EntityPropertyPanel::renderTransformDetails() {
  const auto &transformComponent =
      context.getComponent<TransformComponent>(entity);

  if (ImGui::CollapsingHeader("Transform")) {
    if (ImGui::BeginTable("table-transformWorld", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableFlags_RowBg)) {

      ImGui::Text("World transform");
      for (uint32_t i = 0; i < 4; ++i) {
        ImGui::TableNextRow();
        for (uint32_t j = 0; j < 4; ++j) {
          ImGui::TableNextColumn();
          ImGui::Text("%f", transformComponent.worldTransform[i][j]);
        }
      }

      ImGui::EndTable();
    }
  }
}

void EntityPropertyPanel::renderLightDetails() {
  auto &light = context.getComponent<LightComponent>(entity).light;

  if (ImGui::CollapsingHeader("Light")) {
    ImGui::Text("Direction");
    const glm::vec3 &direction = light->getDirection();
    ImVec4 imguiDirection(direction.x, direction.y, direction.z, 1.0f);
    if (ImGui::InputFloat3("", (float *)&imguiDirection)) {
      light->setDirection(
          {imguiDirection.x, imguiDirection.y, imguiDirection.z});
    }
  }
}
