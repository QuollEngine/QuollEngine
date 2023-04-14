#include "liquid/core/Base.h"
#include "liquid/imgui/ImguiUtils.h"

#include "EntityPanel.h"

#include "Widgets.h"
#include "FontAwesome.h"

#include "liquidator/actions/EntityNameActions.h"
#include "liquidator/actions/EntityTransformActions.h"
#include "liquidator/actions/EntityTextActions.h"
#include "liquidator/actions/EntityRigidBodyActions.h"
#include "liquidator/actions/EntityLightActions.h"
#include "liquidator/actions/EntityCameraActions.h"
#include "liquidator/actions/EntityCollidableActions.h"
#include "liquidator/actions/EntitySkeletonActions.h"
#include "liquidator/actions/EntityAudioActions.h"
#include "liquidator/actions/EntityScriptingActions.h"
#include "liquidator/actions/SceneActions.h"

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

void EntityPanel::render(WorkspaceState &state, ActionExecutor &actionExecutor,
                         Entity entity) {
  if (entity == Entity::Null) {
    return;
  }

  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  setSelectedEntity(scene, entity);

  if (auto _ = widgets::Window("Entity")) {
    if (scene.entityDatabase.exists(mSelectedEntity)) {
      renderName(scene, actionExecutor);
      renderTransform(scene, actionExecutor);
      renderText(scene, state.assetRegistry, actionExecutor);
      renderMesh(scene, state.assetRegistry);
      renderLight(scene, actionExecutor);
      renderCamera(state, scene, actionExecutor);
      renderAnimation(state, scene, state.assetRegistry);
      renderSkeleton(scene, actionExecutor);
      renderCollidable(scene, actionExecutor);
      renderRigidBody(scene, actionExecutor);
      renderAudio(scene, state.assetRegistry);
      renderScripting(scene, state.assetRegistry, actionExecutor);
      renderAddComponent(scene, state.assetRegistry, actionExecutor);
      handleDragAndDrop(state.assetRegistry, actionExecutor);
    }
  }
}

void EntityPanel::setSelectedEntity(Scene &scene, Entity entity) {
  if (mSelectedEntity != entity) {
    mSelectedEntity = entity;
    mName = scene.entityDatabase.get<Name>(mSelectedEntity);
  }
}

void EntityPanel::renderName(Scene &scene, ActionExecutor &actionExecutor) {
  static const String SectionName = String(fa::Circle) + "  Name";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    String name = scene.entityDatabase.get<Name>(mSelectedEntity).name;

    if (widgets::Input("", mName.name)) {
      if (!mName.name.empty()) {
        actionExecutor.execute(
            std::make_unique<EntitySetName>(mSelectedEntity, mName));
      }

      mName = scene.entityDatabase.get<Name>(mSelectedEntity);
    }
  }
}

void EntityPanel::renderLight(Scene &scene, ActionExecutor &actionExecutor) {
  bool hasDirectionalLight =
      scene.entityDatabase.has<DirectionalLight>(mSelectedEntity);
  bool hasPointLight = scene.entityDatabase.has<PointLight>(mSelectedEntity);

  if (!hasDirectionalLight && !hasPointLight) {
    return;
  }

  static const String SectionName = String(fa::Lightbulb) + "  Light";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    ImGui::Text("Type");

    String selected = hasDirectionalLight
                          ? "Directional"
                          : (hasPointLight ? "Point" : "Unknown");

    if (ImGui::BeginCombo("###LightType", selected.c_str(), 0)) {
      if (ImGui::Selectable("Directional", &hasDirectionalLight)) {
        actionExecutor.execute(std::make_unique<EntitySetDirectionalLight>(
            mSelectedEntity, DirectionalLight{}));
      }
      if (ImGui::Selectable("Point light", &hasPointLight)) {
        actionExecutor.execute(std::make_unique<EntitySetPointLight>(
            mSelectedEntity, PointLight{}));
      }
      ImGui::EndCombo();
    }

    renderDirectionalLight(scene, actionExecutor);
    renderPointLight(scene, actionExecutor);
  }
}

void EntityPanel::renderDirectionalLight(Scene &scene,
                                         ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<DirectionalLight>(mSelectedEntity)) {
    return;
  }

  auto &component = scene.entityDatabase.get<DirectionalLight>(mSelectedEntity);

  ImGui::Text("Direction");
  ImGui::Text("%.3f %.3f %.3f", component.direction.x, component.direction.y,
              component.direction.z);

  bool sendAction = false;

  glm::vec4 color = component.color;
  if (widgets::InputColor("Color", color)) {
    if (!mDirectionalLight.has_value()) {
      mDirectionalLight = component;
    }

    component.color = color;
  }

  sendAction |= ImGui::IsItemDeactivatedAfterEdit();

  float intensity = component.intensity;
  if (widgets::Input("Intensity", intensity, false)) {
    if (!mDirectionalLight.has_value()) {
      mDirectionalLight = component;
    }
    component.intensity = intensity;

    sendAction = true;
  }

  if (mDirectionalLight.has_value() && sendAction) {
    actionExecutor.execute(std::make_unique<EntitySetDirectionalLight>(
        mSelectedEntity, component));

    mDirectionalLight.reset();
  }

  bool castShadows =
      scene.entityDatabase.has<CascadedShadowMap>(mSelectedEntity);
  if (ImGui::Checkbox("Cast shadows", &castShadows)) {
    if (castShadows) {
      actionExecutor.execute(
          std::make_unique<EntityEnableCascadedShadowMap>(mSelectedEntity));
    } else {
      actionExecutor.execute(
          std::make_unique<EntityDisableCascadedShadowMap>(mSelectedEntity));
    }
  }

  if (castShadows) {
    bool sendAction = false;
    auto &component =
        scene.entityDatabase.get<CascadedShadowMap>(mSelectedEntity);

    bool softShadows = component.softShadows;
    if (ImGui::Checkbox("Soft shadows", &softShadows)) {
      if (!mCascadedShadowMap.has_value()) {
        mCascadedShadowMap = component;
      }

      component.softShadows = softShadows;
      sendAction = true;
    }

    float splitLambda = component.splitLambda;
    if (widgets::Input("Split lambda", splitLambda, false)) {
      splitLambda = glm::clamp(splitLambda, 0.0f, 1.0f);

      if (!mCascadedShadowMap.has_value()) {
        mCascadedShadowMap = component;
      }

      component.splitLambda = splitLambda;
      sendAction = true;
    }

    int32_t numCascades = static_cast<int32_t>(component.numCascades);
    ImGui::Text("Number of cascades");
    if (ImGui::DragInt("###NumberOfCascades", &numCascades, 0.5f, 1,
                       static_cast<int32_t>(component.MaxCascades))) {
      if (!mCascadedShadowMap.has_value()) {
        mCascadedShadowMap = component;
      }
      component.numCascades = static_cast<uint32_t>(numCascades);
      sendAction = true;
    }

    sendAction |= ImGui::IsItemDeactivatedAfterEdit();

    if (mCascadedShadowMap.has_value() && sendAction) {
      actionExecutor.execute(std::make_unique<EntitySetCascadedShadowMap>(
          mSelectedEntity, component));

      mCascadedShadowMap.reset();
    }
  }
}

void EntityPanel::renderPointLight(Scene &scene,
                                   ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<PointLight>(mSelectedEntity)) {
    return;
  }

  auto &component = scene.entityDatabase.get<PointLight>(mSelectedEntity);

  bool sendAction = false;

  glm::vec4 color = component.color;
  if (widgets::InputColor("Color", color)) {
    if (!mPointLight.has_value()) {
      mPointLight = component;
    }

    component.color = color;
  }

  sendAction |= ImGui::IsItemDeactivatedAfterEdit();

  float intensity = component.intensity;
  if (widgets::Input("Intensity (in candelas)", intensity, false)) {
    if (!mDirectionalLight.has_value()) {
      mPointLight = component;
    }
    component.intensity = intensity;

    sendAction = true;
  }

  float range = component.range;
  if (widgets::Input("Range", range, false)) {
    if (!mDirectionalLight.has_value()) {
      mPointLight = component;
    }
    component.range = range;

    sendAction = true;
  }

  if (mPointLight.has_value() && sendAction) {
    actionExecutor.execute(
        std::make_unique<EntitySetPointLight>(mSelectedEntity, component));

    mPointLight.reset();
  }
}

void EntityPanel::renderCamera(WorkspaceState &state, Scene &scene,
                               ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<PerspectiveLens>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Video) + "  Camera";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        scene.entityDatabase.get<PerspectiveLens>(mSelectedEntity);

    bool sendAction = false;

    float fovY = component.fovY;
    if (widgets::Input("FOV", fovY, false)) {
      if (fovY < 0.0f) {
        fovY = 0.0f;
      }

      if (!mPerspectiveLens.has_value()) {
        mPerspectiveLens = component;
      }
      component.fovY = fovY;

      sendAction = true;
    }

    float near = component.near;
    if (widgets::Input("Near", near, false)) {
      if (near < 0.0f) {
        near = 0.0f;
      }

      if (!mPerspectiveLens.has_value()) {
        mPerspectiveLens = component;
      }

      component.near = near;

      sendAction = true;
    }

    float far = component.far;
    if (widgets::Input("Far", far, false)) {
      if (far < 0.0f) {
        far = 0.0f;
      }

      if (!mPerspectiveLens.has_value()) {
        mPerspectiveLens = component;
      }

      component.far = far;

      sendAction = true;
    }

    if (sendAction && mPerspectiveLens.has_value()) {
      actionExecutor.execute(std::make_unique<EntitySetPerspectiveLens>(
          mSelectedEntity, component));

      mPerspectiveLens.reset();
    }

    ImGui::Text("Aspect Ratio");
    static constexpr float MinCustomAspectRatio = 0.01f;
    static constexpr float MaxCustomAspectRatio = 1000.0f;

    bool hasViewportAspectRatio =
        scene.entityDatabase.has<AutoAspectRatio>(mSelectedEntity);

    if (ImGui::BeginCombo("###AspectRatioType",
                          hasViewportAspectRatio ? "Viewport ratio" : "Custom",
                          0)) {

      if (ImGui::Selectable("Viewport ratio")) {
        actionExecutor.execute(
            std::make_unique<EntitySetCameraAutoAspectRatio>(mSelectedEntity));
      }

      if (ImGui::Selectable("Custom")) {
        actionExecutor.execute(
            std::make_unique<EntitySetCameraCustomAspectRatio>(
                mSelectedEntity));
      }

      ImGui::EndCombo();
    }

    if (!hasViewportAspectRatio) {
      ImGui::Text("Custom aspect ratio");
      float aspectRatio = component.aspectRatio;
      if (ImGui::DragFloat("###CustomAspectRatio", &aspectRatio,
                           MinCustomAspectRatio, MinCustomAspectRatio,
                           MaxCustomAspectRatio, "%.2f")) {

        if (!mPerspectiveLens.has_value()) {
          mPerspectiveLens = component;
        }

        component.aspectRatio = aspectRatio;
      }

      if (ImGui::IsItemDeactivatedAfterEdit()) {
        actionExecutor.execute(std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component));
      }
    }

    if (scene.activeCamera != mSelectedEntity) {
      if (ImGui::Button("Set as starting camera")) {
        actionExecutor.execute(
            std::make_unique<SceneSetStartingCamera>(mSelectedEntity));
      }
    } else {
      ImGui::Text("Is the starting camera");
    }
  }
}

void EntityPanel::renderTransform(Scene &scene,
                                  ActionExecutor &actionExecutor) {

  if (!scene.entityDatabase.has<LocalTransform>(mSelectedEntity) ||
      !scene.entityDatabase.has<WorldTransform>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Transform";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component = scene.entityDatabase.get<LocalTransform>(mSelectedEntity);
    auto &world = scene.entityDatabase.get<WorldTransform>(mSelectedEntity);

    bool sendAction = false;

    auto localPosition = component.localPosition;
    if (widgets::Input("Position", component.localPosition, false)) {
      if (!mLocalTransform.has_value()) {
        mLocalTransform = component;
      }

      component.localPosition = localPosition;
      sendAction = true;
    }

    glm::vec3 euler{};
    glm::extractEulerAngleXYZ(glm::toMat4(component.localRotation), euler.x,
                              euler.y, euler.z);
    euler = glm::degrees(euler);

    if (widgets::Input("Rotation", euler, false)) {
      if (!mLocalTransform.has_value()) {
        mLocalTransform = component;
      }

      auto eulerRadians = glm::radians(euler);
      component.localRotation = glm::toQuat(
          glm::eulerAngleXYZ(eulerRadians.x, eulerRadians.y, eulerRadians.z));

      sendAction = true;
    }

    auto localScale = component.localScale;
    if (widgets::Input("Scale", localScale, false)) {
      if (!mLocalTransform.has_value()) {
        mLocalTransform = component;
      }

      component.localScale = localScale;
      sendAction = true;
    }

    if (sendAction && mLocalTransform.has_value()) {
      actionExecutor.execute(std::make_unique<EntitySetLocalTransform>(
          mSelectedEntity, component));

      mLocalTransform.reset();
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

void EntityPanel::renderMesh(Scene &scene, AssetRegistry &assetRegistry) {
  static const String SectionName = String(fa::Cubes) + "  Mesh";

  if (scene.entityDatabase.has<Mesh>(mSelectedEntity)) {
    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle = scene.entityDatabase.get<Mesh>(mSelectedEntity).handle;

      const auto &asset = assetRegistry.getMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }
  }

  if (scene.entityDatabase.has<SkinnedMesh>(mSelectedEntity)) {
    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle =
          scene.entityDatabase.get<SkinnedMesh>(mSelectedEntity).handle;

      const auto &asset = assetRegistry.getSkinnedMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableSkinnedMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }
  }
}

void EntityPanel::renderSkeleton(Scene &scene, ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Skeleton>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Bone) + "  Skeleton";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    bool showBones = scene.entityDatabase.has<SkeletonDebug>(mSelectedEntity);

    if (ImGui::Checkbox("Show bones", &showBones)) {
      actionExecutor.execute(
          std::make_unique<EntityToggleSkeletonDebugBones>(mSelectedEntity));
    }
  }
}

void EntityPanel::renderAnimation(WorkspaceState &state, Scene &scene,
                                  AssetRegistry &assetRegistry) {
  if (!scene.entityDatabase.has<Animator>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Animation";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component = scene.entityDatabase.get<Animator>(mSelectedEntity);
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

    if (state.mode == WorkspaceMode::Simulation) {
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

void EntityPanel::renderCollidable(Scene &scene,
                                   ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Collidable>(mSelectedEntity)) {
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

    auto &collidable = scene.entityDatabase.get<Collidable>(mSelectedEntity);

    if (ImGui::BeginCombo(
            "###SelectGeometryType",
            getGeometryName(collidable.geometryDesc.type).c_str())) {

      for (auto type : types) {
        if (type != collidable.geometryDesc.type &&
            ImGui::Selectable(getGeometryName(type).c_str())) {
          actionExecutor.execute(
              std::make_unique<EntitySetCollidableType>(mSelectedEntity, type));
        }
      }
      ImGui::EndCombo();
    }

    bool sendAction = false;

    if (collidable.geometryDesc.type == PhysicsGeometryType::Box) {
      auto &box = std::get<PhysicsGeometryBox>(collidable.geometryDesc.params);
      auto halfExtents = box.halfExtents;

      if (widgets::Input("Half extents", halfExtents, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        box.halfExtents = halfExtents;
        sendAction = true;
      }
    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Sphere) {
      auto &sphere =
          std::get<PhysicsGeometrySphere>(collidable.geometryDesc.params);
      float radius = sphere.radius;

      if (widgets::Input("Radius", radius, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        sphere.radius = radius;
        sendAction = true;
      }
    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Capsule) {
      auto &capsule =
          std::get<PhysicsGeometryCapsule>(collidable.geometryDesc.params);
      float radius = capsule.radius;
      float halfHeight = capsule.halfHeight;

      if (widgets::Input("Radius", radius, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        capsule.radius = radius;
        sendAction = true;
      }

      if (widgets::Input("Half height", halfHeight, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        capsule.halfHeight = halfHeight;
        sendAction = true;
      }
    }

    {
      auto &material = collidable.materialDesc;

      float dynamicFriction = material.dynamicFriction;

      if (widgets::Input("Dynamic friction", dynamicFriction, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        material.dynamicFriction = dynamicFriction;
        sendAction = true;
      }

      float restitution = material.restitution;
      if (widgets::Input("Restitution", restitution, false)) {
        if (restitution > 1.0f) {
          restitution = 1.0f;
        }

        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        material.restitution = restitution;
        sendAction = true;
      }

      float staticFriction = material.staticFriction;
      if (widgets::Input("Static friction", staticFriction, false)) {
        if (!mCollidable.has_value()) {
          mCollidable = collidable;
        }

        material.staticFriction = staticFriction;
        sendAction = true;
      }
    }

    if (sendAction && mCollidable.has_value()) {
      actionExecutor.execute(
          std::make_unique<EntitySetCollidable>(mSelectedEntity, collidable));
      mCollidable.reset();
    }
  }
}

void EntityPanel::renderRigidBody(Scene &scene,
                                  ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<RigidBody>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + "  Rigid body";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &rigidBody = scene.entityDatabase.get<RigidBody>(mSelectedEntity);

    bool sendAction = false;
    float mass = rigidBody.dynamicDesc.mass;
    if (widgets::Input("Mass", mass, false)) {
      if (!mRigidBody.has_value()) {
        mRigidBody = rigidBody;
      }

      rigidBody.dynamicDesc.mass = mass;
      sendAction = true;
    }

    glm::vec3 inertia = rigidBody.dynamicDesc.inertia;
    if (widgets::Input("Inertia", inertia, false)) {
      if (!mRigidBody.has_value()) {
        mRigidBody = rigidBody;
      }

      rigidBody.dynamicDesc.inertia = inertia;
      sendAction = true;
    }

    ImGui::Text("Apply gravity");
    bool applyGravity = rigidBody.dynamicDesc.applyGravity;
    if (ImGui::Checkbox("Apply gravity###ApplyGravity", &applyGravity)) {
      if (!mRigidBody.has_value()) {
        mRigidBody = rigidBody;
      }

      rigidBody.dynamicDesc.applyGravity = applyGravity;
      sendAction = true;
    }

    if (sendAction && mRigidBody.has_value()) {
      actionExecutor.execute(
          std::make_unique<EntitySetRigidBody>(mSelectedEntity, rigidBody));

      mRigidBody.reset();
    }
  }
}

void EntityPanel::renderText(Scene &scene, AssetRegistry &assetRegistry,
                             ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Text>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Font) + "  Text";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &text = scene.entityDatabase.get<Text>(mSelectedEntity);

    const auto &fonts = assetRegistry.getFonts().getAssets();

    static constexpr float ContentInputHeight = 100.0f;
    bool sendAction = false;

    ImGui::Text("Content");
    String tmpText = text.text;
    if (ImguiMultilineInputText(
            "###InputContent", tmpText,
            ImVec2(ImGui::GetWindowWidth(), ContentInputHeight), 0)) {
      if (!mText.has_value()) {
        mText = text;
      }

      text.text = tmpText;
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
      sendAction = true;
    }

    float lineHeight = text.lineHeight;
    if (widgets::Input("Line height", lineHeight, false)) {
      if (!mText.has_value()) {
        mText = text;
      }

      text.lineHeight = lineHeight;

      sendAction = true;
    }

    ImGui::Text("Select font");
    if (ImGui::BeginCombo("###SelectFont", fonts.at(text.font).name.c_str(),
                          0)) {
      for (const auto &[handle, data] : fonts) {
        bool selectable = handle == text.font;

        const auto &fontName = data.name;

        if (ImGui::Selectable(fontName.c_str(), &selectable)) {
          if (!mText.has_value()) {
            mText = text;
          }

          text.font = handle;
          sendAction = true;
        }
      }
      ImGui::EndCombo();
    }

    if (sendAction && mText.has_value()) {
      actionExecutor.execute(
          std::make_unique<EntitySetText>(mSelectedEntity, text));

      mText.reset();
    }
  }
}

void EntityPanel::renderAudio(Scene &scene, AssetRegistry &assetRegistry) {
  if (!scene.entityDatabase.has<AudioSource>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Music) + "  Audio";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &audio = scene.entityDatabase.get<AudioSource>(mSelectedEntity);
    const auto &asset = assetRegistry.getAudios().getAsset(audio.source);

    ImGui::Text("Name: %s", asset.name.c_str());
  }
}

void EntityPanel::renderScripting(Scene &scene, AssetRegistry &assetRegistry,
                                  ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Script>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Scroll) + "  Script";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &script = scene.entityDatabase.get<Script>(mSelectedEntity);
    const auto &asset = assetRegistry.getLuaScripts().getAsset(script.handle);

    ImGui::Text("Name: %s", asset.name.c_str());

    if (!asset.data.variables.empty()) {
      ImGui::Text("Variables");

      if (script.started) {
        widgets::Table table("scriptVariables", 3);
        table.row("Name", "Type", "Value");
        for (const auto &[name, variable] : asset.data.variables) {

          String type = "Unknown";
          if (variable.type == LuaScriptVariableType::String) {
            type = "String";
          } else if (variable.type == LuaScriptVariableType::AssetPrefab) {
            type = "Prefab";
          }

          String value;
          if (!script.variables.contains(name) ||
              !script.variables.at(name).isType(variable.type)) {
            // Do nothing
          } else if (script.variables.at(name).isType(
                         LuaScriptVariableType::String)) {
            value = script.variables.at(name).get<String>();
          } else if (script.variables.at(name).isType(
                         LuaScriptVariableType::AssetPrefab)) {
            auto handle = script.variables.at(name).get<PrefabAssetHandle>();
            value = assetRegistry.getPrefabs().getAsset(handle).name;
          }

          table.row(name, type, value);
        }
      } else {
        for (const auto &[name, variable] : asset.data.variables) {
          LuaScriptInputVariable existingVariable;
          if (mSetScriptVariable &&
              mSetScriptVariable->getValue().isType(variable.type) &&
              mSetScriptVariable->getName() == name) {
            existingVariable = mSetScriptVariable->getValue();
          } else if (script.variables.contains(name) &&
                     script.variables.at(name).isType(variable.type)) {
            existingVariable = script.variables.at(name);
          }

          if (variable.type == LuaScriptVariableType::String) {
            auto it = script.variables.find(name);

            auto value = existingVariable.isType(LuaScriptVariableType::String)
                             ? existingVariable.get<String>()
                             : "";

            if (widgets::Input(name, value, false)) {
              if (!mSetScriptVariable) {
                mSetScriptVariable.reset(
                    new EntitySetScriptVariable(mSelectedEntity, name, value));
              }

              mSetScriptVariable->setValue(value);
            }

          } else if (variable.type == LuaScriptVariableType::AssetPrefab) {
            ImGui::Text("%s", name.c_str());
            auto value =
                existingVariable.isType(LuaScriptVariableType::AssetPrefab)
                    ? existingVariable.get<PrefabAssetHandle>()
                    : PrefabAssetHandle::Invalid;

            const auto width = ImGui::GetWindowContentRegionWidth();
            const float halfWidth = width * 0.5f;
            if (value == PrefabAssetHandle::Invalid) {
              ImGui::Button("Drag prefab here", ImVec2(width, halfWidth));
            } else {
              String buttonLabel =
                  "Replace current prefab: " +
                  assetRegistry.getPrefabs().getAsset(value).name;
              ImGui::Button(buttonLabel.c_str(), ImVec2(width, halfWidth));
            }

            if (ImGui::BeginDragDropTarget()) {
              if (auto *payload = ImGui::AcceptDragDropPayload(
                      getAssetTypeString(AssetType::Prefab).c_str())) {
                auto handle = *static_cast<PrefabAssetHandle *>(payload->Data);
                mSetScriptVariable.reset(
                    new EntitySetScriptVariable(mSelectedEntity, name, handle));
              }
            }
          }

          if (mSetScriptVariable) {
            actionExecutor.execute(std::move(mSetScriptVariable));
          }
        }
      }
    }
  }
}

void EntityPanel::renderAddComponent(Scene &scene, AssetRegistry &assetRegistry,
                                     ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.exists(mSelectedEntity)) {
    return;
  }

  bool hasAllComponents =
      scene.entityDatabase.has<LocalTransform>(mSelectedEntity) &&
      scene.entityDatabase.has<RigidBody>(mSelectedEntity) &&
      scene.entityDatabase.has<Collidable>(mSelectedEntity) &&
      scene.entityDatabase.has<DirectionalLight>(mSelectedEntity) &&
      scene.entityDatabase.has<PerspectiveLens>(mSelectedEntity);

  if (hasAllComponents)
    return;

  if (ImGui::Button("Add component")) {
    ImGui::OpenPopup("AddComponentPopup");
  }

  if (ImGui::BeginPopup("AddComponentPopup")) {
    if (!scene.entityDatabase.has<LocalTransform>(mSelectedEntity) &&
        ImGui::Selectable("Transform")) {
      actionExecutor.execute(std::make_unique<EntitySetLocalTransform>(
          mSelectedEntity, LocalTransform{}));
    }

    if (!scene.entityDatabase.has<RigidBody>(mSelectedEntity) &&
        ImGui::Selectable("Rigid body")) {
      actionExecutor.execute(
          std::make_unique<EntitySetRigidBody>(mSelectedEntity, RigidBody{}));
    }

    if (!scene.entityDatabase.has<Collidable>(mSelectedEntity) &&
        ImGui::Selectable("Collidable")) {
      actionExecutor.execute(std::make_unique<EntitySetCollidableType>(
          mSelectedEntity, PhysicsGeometryType::Box));
    }

    if (!scene.entityDatabase.has<DirectionalLight>(mSelectedEntity) &&
        !scene.entityDatabase.has<PointLight>(mSelectedEntity)) {
      if (ImGui::Selectable("Directional light")) {
        actionExecutor.execute(std::make_unique<EntitySetDirectionalLight>(
            mSelectedEntity, liquid::DirectionalLight{}));
      }

      if (ImGui::Selectable("Point light")) {
        actionExecutor.execute(std::make_unique<EntitySetPointLight>(
            mSelectedEntity, liquid::PointLight{}));
      }
    }

    if (!scene.entityDatabase.has<PerspectiveLens>(mSelectedEntity) &&
        ImGui::Selectable("Camera")) {
      actionExecutor.execute(std::make_unique<EntitySetPerspectiveLens>(
          mSelectedEntity, liquid::PerspectiveLens{}));
      actionExecutor.execute(
          std::make_unique<EntitySetCameraAutoAspectRatio>(mSelectedEntity));
    }

    if (!scene.entityDatabase.has<Text>(mSelectedEntity) &&
        ImGui::Selectable("Text")) {
      Text text{"Hello world"};
      text.font = assetRegistry.getDefaultObjects().defaultFont;
      actionExecutor.execute(
          std::make_unique<EntitySetText>(mSelectedEntity, text));
    }

    ImGui::EndPopup();
  }
}

void EntityPanel::handleDragAndDrop(AssetRegistry &assetRegistry,
                                    ActionExecutor &actionExecutor) {
  const auto width = ImGui::GetWindowContentRegionWidth();
  const float halfWidth = width * 0.5f;

  ImGui::Button("Drag asset here", ImVec2(width, halfWidth));

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Audio).c_str())) {
      auto asset = *static_cast<AudioAssetHandle *>(payload->Data);

      actionExecutor.execute(
          std::make_unique<EntitySetAudio>(mSelectedEntity, asset));
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::LuaScript).c_str())) {
      auto asset = *static_cast<LuaScriptAssetHandle *>(payload->Data);

      actionExecutor.execute(
          std::make_unique<EntitySetScript>(mSelectedEntity, asset));
    }

    ImGui::EndDragDropTarget();
  }
}

} // namespace liquid::editor
