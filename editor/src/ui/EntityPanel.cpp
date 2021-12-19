#include "EntityPanel.h"

#include <imgui.h>

namespace liquidator {

EntityPanel::EntityPanel(liquid::EntityContext &entityContext)
    : context(entityContext) {}

void EntityPanel::render(SceneManager &sceneManager) {
  if (!context.hasEntity(selectedEntity)) {
    return;
  }

  liquid::String name;
  if (context.hasComponent<liquid::NameComponent>(selectedEntity)) {
    name = context.getComponent<liquid::NameComponent>(selectedEntity).name +
           "###" + std::to_string(selectedEntity);
  } else {
    name = "Entity: " + std::to_string(selectedEntity);
  }

  bool open = true;
  if (ImGui::Begin(name.c_str(), &open)) {
    renderName();
    renderLight();
    renderTransform();
    ImGui::End();
  }

  if (!open) {
    selectedEntity = liquid::ENTITY_MAX;
  }
}

void EntityPanel::setSelectedEntity(liquid::Entity entity) {
  selectedEntity = entity;
  if (context.hasComponent<liquid::NameComponent>(selectedEntity)) {
  }
}

void EntityPanel::renderName() {
  if (!context.hasComponent<liquid::NameComponent>(selectedEntity)) {
    return;
  }

  auto &component = context.getComponent<liquid::NameComponent>(selectedEntity);

  if (ImGui::CollapsingHeader("Name")) {
    if (ImGui::InputText(
            "##Input", const_cast<char *>(component.name.c_str()),
            component.name.capacity() + 1, ImGuiInputTextFlags_CallbackResize,
            [](ImGuiInputTextCallbackData *data) -> int {
              if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                liquid::String *str =
                    static_cast<liquid::String *>(data->UserData);

                str->resize(data->BufTextLen);
                data->Buf = const_cast<char *>(str->c_str());
              }
              return 0;
            },
            &component.name)) {
    }
  }
}

void EntityPanel::renderLight() {
  if (!context.hasComponent<liquid::LightComponent>(selectedEntity)) {
    return;
  }

  auto &component =
      context.getComponent<liquid::LightComponent>(selectedEntity);

  if (ImGui::CollapsingHeader("Light")) {
    constexpr size_t VEC3_ARRAY_SIZE = 3;
    constexpr size_t VEC4_ARRAY_SIZE = 4;
    ImGui::Text("Type: %s", component.light->getTypeName().c_str());

    ImGui::Text("Direction");
    const glm::vec3 &direction = component.light->getDirection();
    std::array<float, VEC3_ARRAY_SIZE> imguiDirection{direction.x, direction.y,
                                                      direction.z};
    if (ImGui::InputFloat3("###InputDirection", imguiDirection.data())) {
      component.light->setDirection(
          {imguiDirection.at(0), imguiDirection.at(1), imguiDirection.at(2)});
    }

    ImGui::Text("Color");
    const glm::vec4 &color = component.light->getColor();
    std::array<float, VEC4_ARRAY_SIZE> imguiColor{color.r, color.g, color.b,
                                                  color.a};
    if (ImGui::ColorEdit4("###InputColor", imguiColor.data())) {
      component.light->setColor({imguiColor.at(0), imguiColor.at(1),
                                 imguiColor.at(2), imguiColor.at(3)});
    }

    ImGui::Text("Intensity");
    float imguiIntensity = component.light->getIntensity();
    if (ImGui::InputFloat("###InputIntensity", &imguiIntensity)) {
      component.light->setIntensity(imguiIntensity);
    }
  }
}

void EntityPanel::renderTransform() {
  if (!context.hasComponent<liquid::TransformComponent>(selectedEntity)) {
    return;
  }

  auto &component =
      context.getComponent<liquid::TransformComponent>(selectedEntity);

  if (ImGui::CollapsingHeader("Transform")) {
    if (ImGui::BeginTable("table-transformLocal", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableFlags_RowBg)) {

      ImGui::Text("Local transform");
      for (glm::mat4::length_type i = 0; i < 4; ++i) {
        ImGui::TableNextRow();
        for (glm::mat4::length_type j = 0; j < 4; ++j) {
          ImGui::TableNextColumn();
          ImGui::Text("%f", component.transformLocal[i][j]);
        }
      }

      ImGui::EndTable();
    }

    if (ImGui::BeginTable("table-transformWorld", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableFlags_RowBg)) {

      ImGui::Text("World transform");
      for (glm::mat4::length_type i = 0; i < 4; ++i) {
        ImGui::TableNextRow();
        for (glm::mat4::length_type j = 0; j < 4; ++j) {
          ImGui::TableNextColumn();
          ImGui::Text("%f", component.transformWorld[i][j]);
        }
      }

      ImGui::EndTable();
    }
  }
}

} // namespace liquidator
