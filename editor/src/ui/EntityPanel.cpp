#include "liquid/core/Base.h"
#include "EntityPanel.h"

#include <imgui.h>

namespace liquidator {

constexpr size_t VEC3_ARRAY_SIZE = 3;
constexpr size_t VEC4_ARRAY_SIZE = 4;

EntityPanel::EntityPanel(liquid::EntityContext &entityContext)
    : context(entityContext) {}

void EntityPanel::render(SceneManager &sceneManager,
                         const liquid::AnimationSystem &animationSystem,
                         liquid::PhysicsSystem &physicsSystem) {
  bool open = true;
  if (ImGui::Begin("Properties", &open)) {
    if (context.hasEntity(selectedEntity)) {
      renderName();
      renderLight();
      renderTransform();
      renderAnimation(animationSystem);
      renderSkeleton();
      renderCollidable();
      renderRigidBody();
    }
    renderAddComponent();
    ImGui::End();
  }

  if (!open) {
    selectedEntity = liquid::ENTITY_MAX;
  }
}

void EntityPanel::setSelectedEntity(liquid::Entity entity) {
  selectedEntity = entity;
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
    ImGui::Text("Position");
    std::array<float, VEC3_ARRAY_SIZE> imguiPosition{component.localPosition.x,
                                                     component.localPosition.y,
                                                     component.localPosition.z};
    if (ImGui::InputFloat3("###InputTransformPosition", imguiPosition.data())) {
      component.localPosition = {imguiPosition.at(0), imguiPosition.at(1),
                                 imguiPosition.at(2)};
    }

    ImGui::Text("Rotation");
    std::array<float, VEC3_ARRAY_SIZE> imguiRotation{component.localRotation.x,
                                                     component.localRotation.y,
                                                     component.localRotation.z};
    if (ImGui::InputFloat3("###InputTransformRotation", imguiRotation.data())) {
      component.localRotation = glm::quat(glm::vec3(
          imguiRotation.at(0), imguiRotation.at(1), imguiRotation.at(2)));
    }

    ImGui::Text("Scale");
    std::array<float, VEC3_ARRAY_SIZE> imguiScale{
        component.localScale.x, component.localScale.y, component.localScale.z};
    if (ImGui::InputFloat3("###InputTransformScale", imguiScale.data())) {
      component.localScale = {imguiScale.at(0), imguiScale.at(1),
                              imguiScale.at(2)};
    }

    ImGui::Text("World Transform");
    if (ImGui::BeginTable("table-transformWorld", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableFlags_RowBg)) {

      ImGui::Text("World transform");
      for (glm::mat4::length_type i = 0; i < 4; ++i) {
        ImGui::TableNextRow();
        for (glm::mat4::length_type j = 0; j < 4; ++j) {
          ImGui::TableNextColumn();
          ImGui::Text("%f", component.worldTransform[i][j]);
        }
      }

      ImGui::EndTable();
    }
  }
}

void EntityPanel::renderSkeleton() {
  if (!context.hasComponent<liquid::SkeletonComponent>(selectedEntity)) {
    return;
  }

  if (ImGui::CollapsingHeader("Skeleton")) {
    if (context.hasComponent<liquid::DebugComponent>(selectedEntity)) {
      auto &component =
          context.getComponent<liquid::DebugComponent>(selectedEntity);

      ImGui::Checkbox("Show bones", &component.showBones);
    }
  }
}

void EntityPanel::renderAnimation(
    const liquid::AnimationSystem &animationSystem) {
  if (!context.hasComponent<liquid::AnimatorComponent>(selectedEntity)) {
    return;
  }

  if (ImGui::CollapsingHeader("Animation")) {
    auto &component =
        context.getComponent<liquid::AnimatorComponent>(selectedEntity);

    const auto &currentAnimation = animationSystem.getAnimation(
        component.animations.at(component.currentAnimation));

    if (ImGui::BeginCombo("###SelectAnimation",
                          currentAnimation.getName().c_str(), 0)) {
      for (size_t i = 0; i < component.animations.size(); ++i) {
        bool selectable = component.currentAnimation == i;

        const auto &animationName =
            animationSystem.getAnimation(component.animations.at(i)).getName();

        if (ImGui::Selectable(animationName.c_str(), &selectable)) {
          component.currentAnimation = static_cast<uint32_t>(i);
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Text("Time");

    float animationTime = component.normalizedTime * currentAnimation.getTime();
    if (ImGui::SliderFloat("###AnimationTime", &animationTime, 0.0f,
                           currentAnimation.getTime())) {
      component.normalizedTime = animationTime / currentAnimation.getTime();
    }

    ImGui::Checkbox("Loop", &component.loop);

    if (!component.playing) {
      if (ImGui::Button("Play")) {
        component.playing = true;
      }
    } else {
      if (ImGui::Button("Pause")) {
        component.playing = false;
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
      component.normalizedTime = 0.0f;
    }
  }
}

/**
 * @brief Get geometry name
 *
 * @param type Geometry type
 * @return String name for geometry
 */
static liquid::String getGeometryName(const liquid::PhysicsGeometryType &type) {
  switch (type) {
  case liquid::PhysicsGeometryType::Box:
    return "Box";
  case liquid::PhysicsGeometryType::Sphere:
    return "Sphere";
  case liquid::PhysicsGeometryType::Capsule:
    return "Capsule";
  case liquid::PhysicsGeometryType::Plane:
    return "Plane";
  default:
    return "Unknown";
  }
}

/**
 * @brief Get defaulty geometry from type
 *
 * @param type Geometry type
 * @return Default geometry parameters
 */
static liquid::PhysicsGeometryParams
getDefaultGeometryFromType(const liquid::PhysicsGeometryType &type) {
  switch (type) {
  case liquid::PhysicsGeometryType::Box:
  default:
    return liquid::PhysicsGeometryBox();
  case liquid::PhysicsGeometryType::Sphere:
    return liquid::PhysicsGeometrySphere();
  case liquid::PhysicsGeometryType::Capsule:
    return liquid::PhysicsGeometryCapsule();
  case liquid::PhysicsGeometryType::Plane:
    return liquid::PhysicsGeometryPlane();
  }
}

void EntityPanel::renderCollidable() {
  if (!context.hasComponent<liquid::CollidableComponent>(selectedEntity)) {
    return;
  }

  if (ImGui::CollapsingHeader("Collidable")) {
    std::array<liquid::PhysicsGeometryType, sizeof(liquid::PhysicsGeometryType)>
        types{
            liquid::PhysicsGeometryType::Box,
            liquid::PhysicsGeometryType::Sphere,
            liquid::PhysicsGeometryType::Capsule,
            liquid::PhysicsGeometryType::Plane,
        };

    auto &collidable =
        context.getComponent<liquid::CollidableComponent>(selectedEntity);

    if (ImGui::BeginCombo(
            "###SelectGeometryType",
            getGeometryName(collidable.geometryDesc.type).c_str())) {

      for (auto type : types) {
        if (type != collidable.geometryDesc.type &&
            ImGui::Selectable(getGeometryName(type).c_str())) {
          collidable.geometryDesc.type = type;
          collidable.geometryDesc.params = getDefaultGeometryFromType(type);
        }
      }
      ImGui::EndCombo();
    }

    if (collidable.geometryDesc.type == liquid::PhysicsGeometryType::Box) {
      auto &box =
          std::get<liquid::PhysicsGeometryBox>(collidable.geometryDesc.params);
      std::array<float, 3> extents{box.halfExtents.x, box.halfExtents.y,
                                   box.halfExtents.z};
      ImGui::Text("Half extents");
      if (ImGui::InputFloat3("###HalfExtents", extents.data())) {
        box.halfExtents.x = extents.at(0);
        box.halfExtents.y = extents.at(1);
        box.halfExtents.z = extents.at(2);
      }
    } else if (collidable.geometryDesc.type ==
               liquid::PhysicsGeometryType::Sphere) {
      auto &sphere = std::get<liquid::PhysicsGeometrySphere>(
          collidable.geometryDesc.params);
      ImGui::Text("Radius");
      ImGui::InputFloat("###Radius", &sphere.radius);
    } else if (collidable.geometryDesc.type ==
               liquid::PhysicsGeometryType::Capsule) {
      auto &capsule = std::get<liquid::PhysicsGeometryCapsule>(
          collidable.geometryDesc.params);
      ImGui::Text("Radius");
      ImGui::InputFloat("###Radius", &capsule.radius);

      ImGui::Text("Half height");
      ImGui::InputFloat("###HalfHeight", &capsule.halfHeight);
    }
  }
}

void EntityPanel::renderRigidBody() {
  if (!context.hasComponent<liquid::RigidBodyComponent>(selectedEntity)) {
    return;
  }

  if (ImGui::CollapsingHeader("Rigid Body")) {
    auto &rigidBody =
        context.getComponent<liquid::RigidBodyComponent>(selectedEntity);

    ImGui::Text("Mass");
    ImGui::InputFloat("###Mass", &rigidBody.dynamicDesc.mass);

    std::array<float, 3> inertia{rigidBody.dynamicDesc.inertia.x,
                                 rigidBody.dynamicDesc.inertia.y,
                                 rigidBody.dynamicDesc.inertia.z};

    if (ImGui::InputFloat3("###Inertia", inertia.data())) {
      rigidBody.dynamicDesc.inertia.x = inertia.at(0);
      rigidBody.dynamicDesc.inertia.y = inertia.at(1);
      rigidBody.dynamicDesc.inertia.z = inertia.at(2);
    }

    rigidBody.actor->getLinearVelocity();

    auto renderRow3 = [](const liquid::String &label,
                         const physx::PxVec3 &value) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(label.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%.2f, %.2f, %.2f", value.x, value.y, value.z);
    };

    auto renderRowQuat = [](const liquid::String &label,
                            const physx::PxQuat &value) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(label.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%.2f, %.2f, %.2f, 0.2f", value.w, value.x, value.y, value.z);
    };

    auto renderScalar = [](const liquid::String &label, float value) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text(label.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%0.2f", value);
    };

    if (ImGui::BeginTable("TableRigidBodyDetails", 2,
                          ImGuiTableFlags_Borders |
                              ImGuiTableColumnFlags_WidthStretch |
                              ImGuiTableFlags_RowBg)) {
      auto *actor = rigidBody.actor;
      renderRow3("Pose position", actor->getGlobalPose().p);
      renderRowQuat("Pose rotation", actor->getGlobalPose().q);
      renderRow3("CMass position", actor->getCMassLocalPose().p);
      renderRowQuat("CMass rotation", actor->getCMassLocalPose().q);
      renderRow3("Inverse inertia tensor",
                 actor->getMassSpaceInvInertiaTensor());
      renderScalar("Linear damping", actor->getLinearDamping());
      renderScalar("Angular damping", actor->getAngularDamping());
      renderRow3("Linear velocity", actor->getLinearVelocity());
      renderRow3("Angular velocity", actor->getAngularVelocity());

      ImGui::EndTable();
    }
  }
}

void EntityPanel::renderAddComponent() {
  if (!context.hasEntity(selectedEntity)) {
    return;
  }

  bool hasAllComponents =
      context.hasComponent<liquid::TransformComponent>(selectedEntity) &&
      context.hasComponent<liquid::RigidBodyComponent>(selectedEntity) &&
      context.hasComponent<liquid::CollidableComponent>(selectedEntity);

  if (hasAllComponents)
    return;

  if (ImGui::Button("Add component")) {
    ImGui::OpenPopup("AddComponentPopup");
  }

  if (ImGui::BeginPopup("AddComponentPopup")) {
    if (!context.hasComponent<liquid::TransformComponent>(selectedEntity) &&
        ImGui::Selectable("Transform")) {
      context.setComponent<liquid::TransformComponent>(selectedEntity, {});
    }

    if (!context.hasComponent<liquid::RigidBodyComponent>(selectedEntity) &&
        ImGui::Selectable("Rigid body")) {
      context.setComponent<liquid::RigidBodyComponent>(selectedEntity, {});
    }

    if (!context.hasComponent<liquid::CollidableComponent>(selectedEntity) &&
        ImGui::Selectable("Collidable")) {
      constexpr glm::vec3 DEFAULT_VALUE(0.5f);

      context.setComponent<liquid::CollidableComponent>(
          selectedEntity, {liquid::PhysicsGeometryType::Box,
                           liquid::PhysicsGeometryBox{DEFAULT_VALUE}});
    }

    ImGui::EndPopup();
  }
}

} // namespace liquidator
