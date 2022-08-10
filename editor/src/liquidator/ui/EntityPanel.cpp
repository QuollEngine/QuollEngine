#include "liquid/core/Base.h"
#include "liquid/imgui/ImguiUtils.h"

#include "EntityPanel.h"

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
 * @brief Multiline Input text for std::string
 *
 * @param label Label
 * @param value String value
 * @param flags Input text flags
 */
static bool ImguiMultilineInputText(const liquid::String &label,
                                    liquid::String &value, const ImVec2 &size,
                                    ImGuiInputTextFlags flags = 0) {
  LIQUID_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0,
                "Do not back callback resize flag");

  flags |= ImGuiInputTextFlags_CallbackResize;

  ImguiInputTextCallbackUserData userData{
      value,
  };
  return ImGui::InputTextMultiline(label.c_str(), value.data(),
                                   value.capacity() + 1, size, flags,
                                   InputTextCallback, &userData);
}

EntityPanel::EntityPanel(EntityManager &entityManager)
    : mEntityManager(entityManager) {}

void EntityPanel::render(EditorManager &editorManager, liquid::Entity entity,
                         liquid::Renderer &renderer,
                         liquid::AssetRegistry &assetRegistry,
                         liquid::PhysicsSystem &physicsSystem) {
  setSelectedEntity(entity);

  if (auto _ = widgets::Window("Entity")) {
    if (mEntityManager.getActiveEntityDatabase().hasEntity(mSelectedEntity)) {
      renderName();
      renderTransform();
      renderText(assetRegistry);
      renderMesh(assetRegistry);
      renderLight();
      renderCamera(editorManager);
      renderAnimation(assetRegistry);
      renderSkeleton();
      renderCollidable();
      renderRigidBody();
      renderAudio(assetRegistry);
      renderScripting(assetRegistry);
      renderAddComponent(assetRegistry);
      handleDragAndDrop(renderer, assetRegistry);
    }
  }
}

void EntityPanel::setSelectedEntity(liquid::Entity entity) {
  if (mSelectedEntity != entity) {
    mSelectedEntity = entity;
    mName = mEntityManager.getActiveEntityDatabase()
                .getComponent<liquid::NameComponent>(mSelectedEntity)
                .name;
  }
}

void EntityPanel::renderName() {
  if (auto _ = widgets::Section("Name")) {
    if (widgets::Input("", mName)) {
      if (!mName.empty()) {
        mEntityManager.setName(mSelectedEntity, mName);
        mEntityManager.save(mSelectedEntity);
      }

      mName = mEntityManager.getActiveEntityDatabase()
                  .getComponent<liquid::NameComponent>(mSelectedEntity)
                  .name;
    }
  }
}

void EntityPanel::renderLight() {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::DirectionalLightComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Light")) {
    auto &component =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::DirectionalLightComponent>(mSelectedEntity);

    ImGui::Text("Type");
    if (ImGui::BeginCombo("###LightType", "Directional", 0)) {
      if (ImGui::Selectable("Directional")) {
      }
      ImGui::EndCombo();
    }

    ImGui::Text("Direction");
    ImGui::Text("%.3f %.3f %.3f", component.direction.x, component.direction.y,
                component.direction.z);

    if (widgets::InputColor("Color", component.color)) {
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Intensity", component.intensity)) {
      mEntityManager.save(mSelectedEntity);
    }
  }
}

void EntityPanel::renderCamera(EditorManager &editorManager) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::PerspectiveLensComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Camera")) {
    auto &component =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::PerspectiveLensComponent>(mSelectedEntity);

    if (widgets::Input("FOV", component.fovY)) {
      if (component.fovY < 0.0f) {
        component.fovY = 0.0f;
      }
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Near", component.near)) {
      if (component.near < 0.0f) {
        component.near = 0.0f;
      }
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Far", component.far)) {
      if (component.far < 0.0f) {
        component.far = 0.0f;
      }
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::Text("Aspect Ratio");
    static constexpr float MinCustomAspectRatio = 0.01f;
    static constexpr float MaxCustomAspectRatio = 1000.0f;

    bool hasViewportAspectRatio =
        mEntityManager.getActiveEntityDatabase()
            .hasComponent<liquid::AutoAspectRatioComponent>(mSelectedEntity);

    if (ImGui::BeginCombo("###AspectRatioType",
                          hasViewportAspectRatio ? "Viewport ratio" : "Custom",
                          0)) {

      if (ImGui::Selectable("Viewport ratio")) {
        mEntityManager.getActiveEntityDatabase()
            .setComponent<liquid::AutoAspectRatioComponent>(mSelectedEntity,
                                                            {});
        mEntityManager.save(mSelectedEntity);
      }

      if (ImGui::Selectable("Custom")) {
        mEntityManager.getActiveEntityDatabase()
            .deleteComponent<liquid::AutoAspectRatioComponent>(mSelectedEntity);
        mEntityManager.save(mSelectedEntity);
      }

      ImGui::EndCombo();
    }

    if (!hasViewportAspectRatio) {
      ImGui::Text("Custom aspect ratio");
      if (ImGui::DragFloat("###CustomAspectRatio", &component.aspectRatio,
                           MinCustomAspectRatio, MinCustomAspectRatio,
                           MaxCustomAspectRatio, "%.2f")) {
      }

      if (ImGui::IsItemDeactivatedAfterEdit()) {
        mEntityManager.save(mSelectedEntity);
      }
    }

    if (!editorManager.isUsingCamera(mSelectedEntity) &&
        ImGui::Button("Set as active camera")) {
      editorManager.setActiveCamera(mSelectedEntity);
    }
  }
}

void EntityPanel::renderTransform() {
  auto &entityDatabase = mEntityManager.getActiveEntityDatabase();

  if (!entityDatabase.hasComponent<liquid::LocalTransformComponent>(
          mSelectedEntity) ||
      !entityDatabase.hasComponent<liquid::WorldTransformComponent>(
          mSelectedEntity)) {
    return;
  }

  if (widgets::Section("Transform")) {
    auto &component =
        entityDatabase.getComponent<liquid::LocalTransformComponent>(
            mSelectedEntity);
    auto &world = entityDatabase.getComponent<liquid::WorldTransformComponent>(
        mSelectedEntity);

    if (widgets::Input("Position", component.localPosition)) {
      mEntityManager.save(mSelectedEntity);
    }

    auto euler = glm::eulerAngles(component.localRotation);
    if (widgets::Input("Rotation", euler)) {
      component.localRotation = glm::quat(euler);
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Scale", component.localScale)) {
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::Text("World Transform");
    if (auto table = widgets::Table("TableTransformWorld", 4)) {
      for (glm::mat4::length_type i = 0; i < 4; ++i) {
        table.row(world.worldTransform[i].x, world.worldTransform[i].y,
                  world.worldTransform[i].z, world.worldTransform[i].w);
      }
    }
  }
}

void EntityPanel::renderMesh(liquid::AssetRegistry &assetRegistry) {
  if (mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::MeshComponent>(mSelectedEntity)) {
    if (auto _ = widgets::Section("Mesh")) {
      auto handle = mEntityManager.getActiveEntityDatabase()
                        .getComponent<liquid::MeshComponent>(mSelectedEntity)
                        .handle;

      const auto &asset = assetRegistry.getMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }
  }

  if (mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::SkinnedMeshComponent>(mSelectedEntity)) {
    if (auto _ = widgets::Section("Mesh")) {
      auto handle =
          mEntityManager.getActiveEntityDatabase()
              .getComponent<liquid::SkinnedMeshComponent>(mSelectedEntity)
              .handle;

      const auto &asset = assetRegistry.getSkinnedMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableSkinnedMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }
  }
}

void EntityPanel::renderSkeleton() {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::SkeletonComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Skeleton")) {
    bool showBones =
        mEntityManager.getActiveEntityDatabase()
            .hasComponent<liquid::SkeletonDebugComponent>(mSelectedEntity);

    if (ImGui::Checkbox("Show bones", &showBones)) {
      mEntityManager.toggleSkeletonDebugForEntity(mSelectedEntity);
    }
  }
}

void EntityPanel::renderAnimation(liquid::AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::AnimatorComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Animation")) {
    auto &component =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::AnimatorComponent>(mSelectedEntity);

    const auto &animations = assetRegistry.getAnimations().getAssets();

    const auto &currentAnimation =
        animations.at(component.animations.at(component.currentAnimation));

    if (ImGui::BeginCombo("###SelectAnimation", currentAnimation.name.c_str(),
                          0)) {
      for (size_t i = 0; i < component.animations.size(); ++i) {
        bool selectable = component.currentAnimation == i;

        const auto &animationName =
            animations.at(component.animations.at(i)).name;

        if (ImGui::Selectable(animationName.c_str(), &selectable)) {
          component.currentAnimation = static_cast<uint32_t>(i);
        }
      }
      ImGui::EndCombo();
    }

    if (mEntityManager.isUsingSimulationDatabase()) {
      ImGui::Text("Time");

      float animationTime =
          component.normalizedTime * currentAnimation.data.time;
      if (ImGui::SliderFloat("###AnimationTime", &animationTime, 0.0f,
                             currentAnimation.data.time)) {
        component.normalizedTime = animationTime / currentAnimation.data.time;
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
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::CollidableComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Collidable")) {
    std::array<liquid::PhysicsGeometryType, sizeof(liquid::PhysicsGeometryType)>
        types{
            liquid::PhysicsGeometryType::Box,
            liquid::PhysicsGeometryType::Sphere,
            liquid::PhysicsGeometryType::Capsule,
            liquid::PhysicsGeometryType::Plane,
        };

    auto &collidable =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::CollidableComponent>(mSelectedEntity);

    if (ImGui::BeginCombo(
            "###SelectGeometryType",
            getGeometryName(collidable.geometryDesc.type).c_str())) {

      for (auto type : types) {
        if (type != collidable.geometryDesc.type &&
            ImGui::Selectable(getGeometryName(type).c_str())) {
          collidable.geometryDesc.type = type;
          collidable.geometryDesc.params = getDefaultGeometryFromType(type);
          mEntityManager.save(mSelectedEntity);
        }
      }
      ImGui::EndCombo();
    }

    if (collidable.geometryDesc.type == liquid::PhysicsGeometryType::Box) {
      auto &box =
          std::get<liquid::PhysicsGeometryBox>(collidable.geometryDesc.params);
      auto halfExtents = box.halfExtents;

      if (widgets::Input("Half extents", box.halfExtents)) {
        mEntityManager.save(mSelectedEntity);
      }
    } else if (collidable.geometryDesc.type ==
               liquid::PhysicsGeometryType::Sphere) {
      auto &sphere = std::get<liquid::PhysicsGeometrySphere>(
          collidable.geometryDesc.params);
      float radius = sphere.radius;

      if (widgets::Input("Radius", sphere.radius)) {
        mEntityManager.save(mSelectedEntity);
      }

    } else if (collidable.geometryDesc.type ==
               liquid::PhysicsGeometryType::Capsule) {
      auto &capsule = std::get<liquid::PhysicsGeometryCapsule>(
          collidable.geometryDesc.params);
      float radius = capsule.radius;
      float halfHeight = capsule.halfHeight;

      if (widgets::Input("Radius", capsule.radius)) {
        mEntityManager.save(mSelectedEntity);
      }

      if (widgets::Input("Half height", capsule.halfHeight)) {
        mEntityManager.save(mSelectedEntity);
      }
    }

    {
      auto &material = collidable.materialDesc;

      if (widgets::Input("Dynamic friction", material.dynamicFriction)) {
        mEntityManager.save(mSelectedEntity);
      }

      if (widgets::Input("Restitution", material.restitution)) {
        if (material.restitution > 1.0f) {
          material.restitution = 1.0f;
        }
        mEntityManager.save(mSelectedEntity);
      }

      if (widgets::Input("Static friction", material.staticFriction)) {
        mEntityManager.save(mSelectedEntity);
      }
    }
  }
}

void EntityPanel::renderRigidBody() {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::RigidBodyComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Rigid body")) {
    auto &rigidBody =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::RigidBodyComponent>(mSelectedEntity);

    if (widgets::Input("Mass", rigidBody.dynamicDesc.mass)) {
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Inertia", rigidBody.dynamicDesc.inertia)) {
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::Text("Apply gravity");
    if (ImGui::Checkbox("Apply gravity###ApplyGravity",
                        &rigidBody.dynamicDesc.applyGravity)) {
      mEntityManager.save(mSelectedEntity);
    }

    if (rigidBody.actor) {
      rigidBody.actor->getLinearVelocity();
      if (auto table = widgets::Table("TableRigidBodyDetails", 2)) {
        auto *actor = rigidBody.actor;

        const auto &pose = actor->getGlobalPose();
        const auto &cmass = actor->getCMassLocalPose();
        const auto &invInertia = actor->getMassSpaceInvInertiaTensor();
        const auto &linearVelocity = actor->getLinearVelocity();
        const auto &angularVelocity = actor->getAngularVelocity();

        table.row("Pose position", glm::vec3(pose.p.x, pose.p.y, pose.p.y));
        table.row("Pose rotation",
                  glm::quat(cmass.q.w, cmass.q.x, cmass.q.y, cmass.q.z));
        table.row("CMass position", glm::vec3(cmass.p.x, cmass.p.y, cmass.p.y));
        table.row("CMass rotation",
                  glm::quat(cmass.q.w, cmass.q.x, cmass.q.y, cmass.q.z));
        table.row("Inverse inertia tensor position",
                  glm::vec3(invInertia.x, invInertia.y, invInertia.y));
        table.row("Linear damping",
                  static_cast<float>(actor->getLinearDamping()));
        table.row("Angular damping",
                  static_cast<float>(actor->getAngularDamping()));
        table.row(
            "Linear velocity",
            glm::vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
        table.row(
            "Angular velocity",
            glm::vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
      }
    }
  }
}

void EntityPanel::renderText(liquid::AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::TextComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Text")) {
    auto &text = mEntityManager.getActiveEntityDatabase()
                     .getComponent<liquid::TextComponent>(mSelectedEntity);

    const auto &fonts = assetRegistry.getFonts().getAssets();

    static constexpr float ContentInputHeight = 100.0f;

    ImGui::Text("Content");
    if (ImguiMultilineInputText(
            "###InputContent", text.text,
            ImVec2(ImGui::GetWindowWidth(), ContentInputHeight), 0)) {
      mEntityManager.save(mSelectedEntity);
    }

    if (widgets::Input("Line height", text.lineHeight)) {
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::Text("Select font");
    if (ImGui::BeginCombo("###SelectFont", fonts.at(text.font).name.c_str(),
                          0)) {
      for (const auto &[handle, data] : fonts) {
        bool selectable = handle == text.font;

        const auto &fontName = data.name;

        if (ImGui::Selectable(fontName.c_str(), &selectable)) {
          text.font = handle;
          mEntityManager.save(mSelectedEntity);
        }
      }
      ImGui::EndCombo();
    }
  }
}

void EntityPanel::renderAudio(liquid::AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::AudioSourceComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Audio")) {
    const auto &audio =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::AudioSourceComponent>(mSelectedEntity);

    const auto &asset = assetRegistry.getAudios().getAsset(audio.source);

    ImGui::Text("Name: %s", asset.name.c_str());
  }
}

void EntityPanel::renderScripting(liquid::AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase()
           .hasComponent<liquid::ScriptingComponent>(mSelectedEntity)) {
    return;
  }

  if (auto _ = widgets::Section("Scripts")) {
    const auto &scripting =
        mEntityManager.getActiveEntityDatabase()
            .getComponent<liquid::ScriptingComponent>(mSelectedEntity);

    const auto &asset =
        assetRegistry.getLuaScripts().getAsset(scripting.handle);

    ImGui::Text("Name: %s", asset.name.c_str());
  }
}

void EntityPanel::renderAddComponent(liquid::AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().hasEntity(mSelectedEntity)) {
    return;
  }

  bool hasAllComponents =
      mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::LocalTransformComponent>(mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::RigidBodyComponent>(mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::CollidableComponent>(mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::DirectionalLightComponent>(mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase()
          .hasComponent<liquid::PerspectiveLensComponent>(mSelectedEntity);

  if (hasAllComponents)
    return;

  if (ImGui::Button("Add component")) {
    ImGui::OpenPopup("AddComponentPopup");
  }

  if (ImGui::BeginPopup("AddComponentPopup")) {
    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::LocalTransformComponent>(mSelectedEntity) &&
        ImGui::Selectable("Transform")) {
      mEntityManager.getActiveEntityDatabase()
          .setComponent<liquid::LocalTransformComponent>(mSelectedEntity, {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::RigidBodyComponent>(mSelectedEntity) &&
        ImGui::Selectable("Rigid body")) {
      mEntityManager.getActiveEntityDatabase()
          .setComponent<liquid::RigidBodyComponent>(mSelectedEntity, {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::CollidableComponent>(mSelectedEntity) &&
        ImGui::Selectable("Collidable")) {
      static constexpr glm::vec3 DefaultValue(0.5f);

      mEntityManager.getActiveEntityDatabase()
          .setComponent<liquid::CollidableComponent>(
              mSelectedEntity, {liquid::PhysicsGeometryType::Box,
                                liquid::PhysicsGeometryBox{DefaultValue}});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::DirectionalLightComponent>(
                 mSelectedEntity) &&
        ImGui::Selectable("Light")) {
      mEntityManager.getActiveEntityDatabase()
          .setComponent<liquid::DirectionalLightComponent>(mSelectedEntity, {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::PerspectiveLensComponent>(mSelectedEntity) &&
        ImGui::Selectable("Camera")) {
      mEntityManager.setCamera(mSelectedEntity, {}, true);
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase()
             .hasComponent<liquid::TextComponent>(mSelectedEntity) &&
        ImGui::Selectable("Text")) {
      liquid::TextComponent text{"Hello world"};
      text.font = assetRegistry.getDefaultObjects().defaultFont;
      mEntityManager.setText(mSelectedEntity, text);
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::EndPopup();
  }
}

void EntityPanel::handleDragAndDrop(liquid::Renderer &renderer,
                                    liquid::AssetRegistry &assetRegistry) {
  const auto width = ImGui::GetWindowContentRegionWidth();
  const float halfWidth = width * 0.5f;

  ImGui::Button("Drag asset here", ImVec2(width, halfWidth));

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            liquid::getAssetTypeString(liquid::AssetType::Mesh).c_str())) {
      auto asset = *static_cast<liquid::MeshAssetHandle *>(payload->Data);

      mEntityManager.setMesh(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            liquid::getAssetTypeString(liquid::AssetType::SkinnedMesh)
                .c_str())) {
      auto asset =
          *static_cast<liquid::SkinnedMeshAssetHandle *>(payload->Data);
      mEntityManager.setSkinnedMesh(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            liquid::getAssetTypeString(liquid::AssetType::Skeleton).c_str())) {
      auto asset = *static_cast<liquid::SkeletonAssetHandle *>(payload->Data);

      mEntityManager.setSkeletonForEntity(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            liquid::getAssetTypeString(liquid::AssetType::Audio).c_str())) {
      auto asset = *static_cast<liquid::AudioAssetHandle *>(payload->Data);

      mEntityManager.setAudio(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            liquid::getAssetTypeString(liquid::AssetType::LuaScript).c_str())) {
      auto asset = *static_cast<liquid::LuaScriptAssetHandle *>(payload->Data);

      mEntityManager.setScript(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::EndDragDropTarget();
  }
}

} // namespace liquidator
