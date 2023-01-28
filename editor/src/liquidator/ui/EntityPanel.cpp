#include "liquid/core/Base.h"
#include "liquid/imgui/ImguiUtils.h"

#include "EntityPanel.h"

#include "Widgets.h"
#include "FontAwesome.h"

namespace liquid::editor {

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
 * @brief Multiline Input text for std::string
 *
 * @param label Label
 * @param value String value
 * @param flags Input text flags
 */
static bool ImguiMultilineInputText(const String &label, String &value,
                                    const ImVec2 &size,
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

void EntityPanel::render(EditorManager &editorManager, Entity entity,
                         Renderer &renderer, AssetManager &assetManager,
                         PhysicsSystem &physicsSystem) {
  setSelectedEntity(entity);

  auto &assetRegistry = assetManager.getAssetRegistry();

  if (auto _ = widgets::Window("Entity")) {
    if (mEntityManager.getActiveEntityDatabase().exists(mSelectedEntity)) {
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

void EntityPanel::setSelectedEntity(Entity entity) {
  if (mSelectedEntity != entity) {
    mSelectedEntity = entity;
    mName = mEntityManager.getActiveEntityDatabase()
                .get<Name>(mSelectedEntity)
                .name;
  }
}

void EntityPanel::renderName() {
  static const String SectionName = String(fa::Circle) + "  Name";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    if (widgets::Input("", mName)) {
      if (!mName.empty()) {
        mEntityManager.setName(mSelectedEntity, mName);
        mEntityManager.save(mSelectedEntity);
      }

      mName = mEntityManager.getActiveEntityDatabase()
                  .get<Name>(mSelectedEntity)
                  .name;
    }
  }
}

void EntityPanel::renderLight() {
  if (!mEntityManager.getActiveEntityDatabase().has<DirectionalLight>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Lightbulb) + "  Light";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        mEntityManager.getActiveEntityDatabase().get<DirectionalLight>(
            mSelectedEntity);

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

    bool castShadows =
        mEntityManager.getActiveEntityDatabase().has<CascadedShadowMap>(
            mSelectedEntity);
    if (ImGui::Checkbox("Cast shadows", &castShadows)) {
      mEntityManager.toggleShadowsForLightEntity(mSelectedEntity);
      mEntityManager.save(mSelectedEntity);
    }

    if (castShadows) {
      auto &component =
          mEntityManager.getActiveEntityDatabase().get<CascadedShadowMap>(
              mSelectedEntity);

      if (ImGui::Checkbox("Soft shadows", &component.softShadows)) {
        mEntityManager.save(mSelectedEntity);
      }

      if (widgets::Input("Split lambda", component.splitLambda)) {
        component.splitLambda = glm::clamp(component.splitLambda, 0.0f, 1.0f);
        mEntityManager.save(mSelectedEntity);
      }

      if (widgets::Input("Number of cascades", component.numCascades)) {
        component.numCascades =
            glm::clamp(component.numCascades, 1u, component.MaxCascades);
        mEntityManager.save(mSelectedEntity);
      }
    }
  }
}

void EntityPanel::renderCamera(EditorManager &editorManager) {
  if (!mEntityManager.getActiveEntityDatabase().has<PerspectiveLens>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Video) + "  Camera";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        mEntityManager.getActiveEntityDatabase().get<PerspectiveLens>(
            mSelectedEntity);

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
        mEntityManager.getActiveEntityDatabase().has<AutoAspectRatio>(
            mSelectedEntity);

    if (ImGui::BeginCombo("###AspectRatioType",
                          hasViewportAspectRatio ? "Viewport ratio" : "Custom",
                          0)) {

      if (ImGui::Selectable("Viewport ratio")) {
        mEntityManager.getActiveEntityDatabase().set<AutoAspectRatio>(
            mSelectedEntity, {});
        mEntityManager.save(mSelectedEntity);
      }

      if (ImGui::Selectable("Custom")) {
        mEntityManager.getActiveEntityDatabase().remove<AutoAspectRatio>(
            mSelectedEntity);
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

    if (mEntityManager.getStartingCamera() != mSelectedEntity) {
      if (ImGui::Button("Set as starting camera")) {
        mEntityManager.setStartingCamera(mSelectedEntity);
      }
    } else {
      ImGui::Text("Is the starting camera");
    }
  }
}

void EntityPanel::renderTransform() {
  auto &entityDatabase = mEntityManager.getActiveEntityDatabase();

  if (!entityDatabase.has<LocalTransform>(mSelectedEntity) ||
      !entityDatabase.has<WorldTransform>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Transform";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component = entityDatabase.get<LocalTransform>(mSelectedEntity);
    auto &world = entityDatabase.get<WorldTransform>(mSelectedEntity);

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

void EntityPanel::renderMesh(AssetRegistry &assetRegistry) {
  static const String SectionName = String(fa::Cubes) + "  Mesh";

  if (mEntityManager.getActiveEntityDatabase().has<Mesh>(mSelectedEntity)) {
    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle = mEntityManager.getActiveEntityDatabase()
                        .get<Mesh>(mSelectedEntity)
                        .handle;

      const auto &asset = assetRegistry.getMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }
  }

  if (mEntityManager.getActiveEntityDatabase().has<SkinnedMesh>(
          mSelectedEntity)) {
    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle = mEntityManager.getActiveEntityDatabase()
                        .get<SkinnedMesh>(mSelectedEntity)
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
  if (!mEntityManager.getActiveEntityDatabase().has<Skeleton>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Bone) + "  Skeleton";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    bool showBones =
        mEntityManager.getActiveEntityDatabase().has<SkeletonDebug>(
            mSelectedEntity);

    if (ImGui::Checkbox("Show bones", &showBones)) {
      mEntityManager.toggleSkeletonDebugForEntity(mSelectedEntity);
    }
  }
}

void EntityPanel::renderAnimation(AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().has<Animator>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Animation";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        mEntityManager.getActiveEntityDatabase().get<Animator>(mSelectedEntity);

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
static String getGeometryName(const PhysicsGeometryType &type) {
  switch (type) {
  case PhysicsGeometryType::Box:
    return "Box";
  case PhysicsGeometryType::Sphere:
    return "Sphere";
  case PhysicsGeometryType::Capsule:
    return "Capsule";
  case PhysicsGeometryType::Plane:
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
static PhysicsGeometryParams
getDefaultGeometryFromType(const PhysicsGeometryType &type) {
  switch (type) {
  case PhysicsGeometryType::Box:
  default:
    return PhysicsGeometryBox();
  case PhysicsGeometryType::Sphere:
    return PhysicsGeometrySphere();
  case PhysicsGeometryType::Capsule:
    return PhysicsGeometryCapsule();
  case PhysicsGeometryType::Plane:
    return PhysicsGeometryPlane();
  }
}

void EntityPanel::renderCollidable() {
  if (!mEntityManager.getActiveEntityDatabase().has<Collidable>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Collidable";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    std::array<PhysicsGeometryType, sizeof(PhysicsGeometryType)> types{
        PhysicsGeometryType::Box,
        PhysicsGeometryType::Sphere,
        PhysicsGeometryType::Capsule,
        PhysicsGeometryType::Plane,
    };

    auto &collidable = mEntityManager.getActiveEntityDatabase().get<Collidable>(
        mSelectedEntity);

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

    if (collidable.geometryDesc.type == PhysicsGeometryType::Box) {
      auto &box = std::get<PhysicsGeometryBox>(collidable.geometryDesc.params);
      auto halfExtents = box.halfExtents;

      if (widgets::Input("Half extents", box.halfExtents)) {
        mEntityManager.save(mSelectedEntity);
      }
    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Sphere) {
      auto &sphere =
          std::get<PhysicsGeometrySphere>(collidable.geometryDesc.params);
      float radius = sphere.radius;

      if (widgets::Input("Radius", sphere.radius)) {
        mEntityManager.save(mSelectedEntity);
      }

    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Capsule) {
      auto &capsule =
          std::get<PhysicsGeometryCapsule>(collidable.geometryDesc.params);
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
  if (!mEntityManager.getActiveEntityDatabase().has<RigidBody>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Rigid body";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &rigidBody = mEntityManager.getActiveEntityDatabase().get<RigidBody>(
        mSelectedEntity);

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

void EntityPanel::renderText(AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().has<Text>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Font) + "  Text";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &text =
        mEntityManager.getActiveEntityDatabase().get<Text>(mSelectedEntity);

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

void EntityPanel::renderAudio(AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().has<AudioSource>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Music) + "  Audio";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &audio =
        mEntityManager.getActiveEntityDatabase().get<AudioSource>(
            mSelectedEntity);

    const auto &asset = assetRegistry.getAudios().getAsset(audio.source);

    ImGui::Text("Name: %s", asset.name.c_str());
  }
}

void EntityPanel::renderScripting(AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().has<Script>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Scroll) + "  Script";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &scripting =
        mEntityManager.getActiveEntityDatabase().get<Script>(mSelectedEntity);

    const auto &asset =
        assetRegistry.getLuaScripts().getAsset(scripting.handle);

    ImGui::Text("Name: %s", asset.name.c_str());
  }
}

void EntityPanel::renderAddComponent(AssetRegistry &assetRegistry) {
  if (!mEntityManager.getActiveEntityDatabase().exists(mSelectedEntity)) {
    return;
  }

  bool hasAllComponents =
      mEntityManager.getActiveEntityDatabase().has<LocalTransform>(
          mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase().has<RigidBody>(
          mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase().has<Collidable>(
          mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase().has<DirectionalLight>(
          mSelectedEntity) &&
      mEntityManager.getActiveEntityDatabase().has<PerspectiveLens>(
          mSelectedEntity);

  if (hasAllComponents)
    return;

  if (ImGui::Button("Add component")) {
    ImGui::OpenPopup("AddComponentPopup");
  }

  if (ImGui::BeginPopup("AddComponentPopup")) {
    if (!mEntityManager.getActiveEntityDatabase().has<LocalTransform>(
            mSelectedEntity) &&
        ImGui::Selectable("Transform")) {
      mEntityManager.getActiveEntityDatabase().set<LocalTransform>(
          mSelectedEntity, {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase().has<RigidBody>(
            mSelectedEntity) &&
        ImGui::Selectable("Rigid body")) {
      mEntityManager.getActiveEntityDatabase().set<RigidBody>(mSelectedEntity,
                                                              {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase().has<Collidable>(
            mSelectedEntity) &&
        ImGui::Selectable("Collidable")) {
      mEntityManager.getActiveEntityDatabase().set<Collidable>(
          mSelectedEntity, {PhysicsGeometryType::Box, PhysicsGeometryBox{}});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase().has<DirectionalLight>(
            mSelectedEntity) &&
        ImGui::Selectable("Light")) {
      mEntityManager.getActiveEntityDatabase().set<DirectionalLight>(
          mSelectedEntity, {});
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase().has<PerspectiveLens>(
            mSelectedEntity) &&
        ImGui::Selectable("Camera")) {
      mEntityManager.setCamera(mSelectedEntity, {}, true);
      mEntityManager.save(mSelectedEntity);
    }

    if (!mEntityManager.getActiveEntityDatabase().has<Text>(mSelectedEntity) &&
        ImGui::Selectable("Text")) {
      Text text{"Hello world"};
      text.font = assetRegistry.getDefaultObjects().defaultFont;
      mEntityManager.setText(mSelectedEntity, text);
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::EndPopup();
  }
}

void EntityPanel::handleDragAndDrop(Renderer &renderer,
                                    AssetRegistry &assetRegistry) {
  const auto width = ImGui::GetWindowContentRegionWidth();
  const float halfWidth = width * 0.5f;

  ImGui::Button("Drag asset here", ImVec2(width, halfWidth));

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Mesh).c_str())) {
      auto asset = *static_cast<MeshAssetHandle *>(payload->Data);

      mEntityManager.setMesh(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::SkinnedMesh).c_str())) {
      auto asset = *static_cast<SkinnedMeshAssetHandle *>(payload->Data);
      mEntityManager.setSkinnedMesh(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Skeleton).c_str())) {
      auto asset = *static_cast<SkeletonAssetHandle *>(payload->Data);

      mEntityManager.setSkeletonForEntity(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Audio).c_str())) {
      auto asset = *static_cast<AudioAssetHandle *>(payload->Data);

      mEntityManager.setAudio(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::LuaScript).c_str())) {
      auto asset = *static_cast<LuaScriptAssetHandle *>(payload->Data);

      mEntityManager.setScript(mSelectedEntity, asset);
      mEntityManager.save(mSelectedEntity);
    }

    ImGui::EndDragDropTarget();
  }
}

} // namespace liquid::editor
