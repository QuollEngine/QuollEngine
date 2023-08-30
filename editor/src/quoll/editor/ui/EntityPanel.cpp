#include "quoll/core/Base.h"
#include "quoll/imgui/ImguiUtils.h"

#include "quoll/editor/actions/EntityNameActions.h"
#include "quoll/editor/actions/EntityTransformActions.h"
#include "quoll/editor/actions/EntityTextActions.h"
#include "quoll/editor/actions/EntityRigidBodyActions.h"
#include "quoll/editor/actions/EntityLightActions.h"
#include "quoll/editor/actions/EntityCameraActions.h"
#include "quoll/editor/actions/EntityCollidableActions.h"
#include "quoll/editor/actions/EntitySkeletonActions.h"
#include "quoll/editor/actions/EntityAudioActions.h"
#include "quoll/editor/actions/EntityScriptingActions.h"
#include "quoll/editor/actions/EntityMeshActions.h"
#include "quoll/editor/actions/EntityMeshRendererActions.h"
#include "quoll/editor/actions/EntitySkinnedMeshRendererActions.h"
#include "quoll/editor/actions/EntityAnimatorActions.h"
#include "quoll/editor/actions/EntitySpriteActions.h"
#include "quoll/editor/actions/SceneActions.h"

#include "EntityPanel.h"

#include "Widgets.h"
#include "FontAwesome.h"

namespace quoll::editor {

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

static void dndEnvironmentAsset(widgets::Section &section, Entity entity,
                                const EnvironmentSkybox &skybox,
                                ActionExecutor &actionExecutor) {
  static constexpr float DropBorderWidth = 3.5f;
  auto &g = *ImGui::GetCurrentContext();

  ImVec2 dropMin(section.getClipRect().Min.x + DropBorderWidth,
                 g.LastItemData.Rect.Min.y + DropBorderWidth);
  ImVec2 dropMax(section.getClipRect().Max.x - DropBorderWidth,
                 g.LastItemData.Rect.Max.y - DropBorderWidth);
  if (ImGui::BeginDragDropTargetCustom(ImRect(dropMin, dropMax),
                                       g.LastItemData.ID)) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Environment).c_str())) {
      auto asset = *static_cast<EnvironmentAssetHandle *>(payload->Data);
      auto newSkybox = skybox;
      newSkybox.texture = asset;
      actionExecutor.execute<EntityDefaultUpdateComponent<EnvironmentSkybox>>(
          entity, skybox, newSkybox);
    }
  }
}

static String getSkyboxTypeLabel(EnvironmentSkyboxType type) {
  if (type == EnvironmentSkyboxType::Color) {
    return "Color";
  }

  if (type == EnvironmentSkyboxType::Texture) {
    return "Texture";
  }

  return "None";
}

void EntityPanel::renderContent(WorkspaceState &state,
                                AssetRegistry &assetRegistry,
                                ActionExecutor &actionExecutor) {
  if (state.selectedEntity == Entity::Null) {
    ImGui::Text("Select an entity in the scene to see properties");
    return;
  }

  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  setSelectedEntity(scene, state.selectedEntity);
  if (scene.entityDatabase.exists(mSelectedEntity)) {
    renderName(scene, actionExecutor);
    renderTransform(scene, actionExecutor);
    renderText(scene, assetRegistry, actionExecutor);
    renderSprite(scene, assetRegistry, actionExecutor);
    renderMesh(scene, assetRegistry, actionExecutor);
    renderMeshRenderer(scene, assetRegistry, actionExecutor);
    renderSkinnedMeshRenderer(scene, assetRegistry, actionExecutor);
    renderDirectionalLight(scene, actionExecutor);
    renderPointLight(scene, actionExecutor);
    renderCamera(state, scene, actionExecutor);
    renderAnimation(state, scene, assetRegistry, actionExecutor);
    renderSkeleton(scene, assetRegistry, actionExecutor);
    renderJointAttachment(scene, actionExecutor);
    renderCollidable(scene, actionExecutor);
    renderRigidBody(scene, actionExecutor);
    renderAudio(scene, assetRegistry, actionExecutor);
    renderScripting(scene, assetRegistry, actionExecutor);
    renderSkybox(scene, assetRegistry, actionExecutor);
    renderEnvironmentLighting(scene, assetRegistry, actionExecutor);
    renderAddComponent(scene, assetRegistry, actionExecutor);
    handleDragAndDrop(scene, assetRegistry, actionExecutor);
  }
}

void EntityPanel::setSelectedEntity(Scene &scene, Entity entity) {
  if (mSelectedEntity != entity) {
    mSelectedEntity = entity;
  }
}

void EntityPanel::renderName(Scene &scene, ActionExecutor &actionExecutor) {
  static const String SectionName = String(fa::Circle) + "  Name";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &name = scene.entityDatabase.get<Name>(mSelectedEntity);

    auto tmpName = name.name;
    if (widgets::Input("", tmpName)) {
      if (!tmpName.empty()) {
        if (!mNameAction) {
          mNameAction = std::make_unique<EntitySetName>(mSelectedEntity, name);
        }

        name.name = tmpName;
      }
    }

    if (mNameAction) {
      mNameAction->setNewComponent(name);
      actionExecutor.execute(std::move(mNameAction));
    }
  }
}

void EntityPanel::renderDirectionalLight(Scene &scene,
                                         ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<DirectionalLight>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Sun) + "  Directional light";
  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        scene.entityDatabase.get<DirectionalLight>(mSelectedEntity);

    ImGui::Text("Direction");
    ImGui::Text("%.3f %.3f %.3f", component.direction.x, component.direction.y,
                component.direction.z);

    bool sendAction = false;

    glm::vec4 color = component.color;
    if (widgets::InputColor("Color", color)) {
      if (!mDirectionalLightAction) {
        mDirectionalLightAction = std::make_unique<EntitySetDirectionalLight>(
            mSelectedEntity, component);
      }

      component.color = color;
    }

    sendAction |= ImGui::IsItemDeactivatedAfterEdit();

    float intensity = component.intensity;
    if (widgets::Input("Intensity", intensity, false)) {
      if (!mDirectionalLightAction) {
        mDirectionalLightAction = std::make_unique<EntitySetDirectionalLight>(
            mSelectedEntity, component);
      }

      component.intensity = intensity;
      sendAction = true;
    }

    if (sendAction && mDirectionalLightAction) {
      mDirectionalLightAction->setNewComponent(component);
      actionExecutor.execute(std::move(mDirectionalLightAction));
    }

    bool castShadows =
        scene.entityDatabase.has<CascadedShadowMap>(mSelectedEntity);
    if (ImGui::Checkbox("Cast shadows", &castShadows)) {
      if (castShadows) {
        actionExecutor.execute<EntityEnableCascadedShadowMap>(mSelectedEntity);
      } else {
        actionExecutor.execute<EntityDisableCascadedShadowMap>(mSelectedEntity);
      }
    }

    castShadows = scene.entityDatabase.has<CascadedShadowMap>(mSelectedEntity);

    if (castShadows) {
      bool sendAction = false;
      auto &component =
          scene.entityDatabase.get<CascadedShadowMap>(mSelectedEntity);

      bool softShadows = component.softShadows;
      if (ImGui::Checkbox("Soft shadows", &softShadows)) {
        if (!mCascadedShadowMapAction) {
          mCascadedShadowMapAction =
              std::make_unique<EntitySetCascadedShadowMap>(mSelectedEntity,
                                                           component);
        }

        component.softShadows = softShadows;
        sendAction = true;
      }

      float splitLambda = component.splitLambda;
      if (widgets::Input("Split lambda", splitLambda, false)) {
        splitLambda = glm::clamp(splitLambda, 0.0f, 1.0f);

        mCascadedShadowMapAction = std::make_unique<EntitySetCascadedShadowMap>(
            mSelectedEntity, component);

        component.splitLambda = splitLambda;
        sendAction = true;
      }

      int32_t numCascades = static_cast<int32_t>(component.numCascades);
      ImGui::Text("Number of cascades");
      if (ImGui::DragInt("###NumberOfCascades", &numCascades, 0.5f, 1,
                         static_cast<int32_t>(component.MaxCascades))) {
        mCascadedShadowMapAction = std::make_unique<EntitySetCascadedShadowMap>(
            mSelectedEntity, component);

        component.numCascades = static_cast<uint32_t>(numCascades);
      }

      sendAction |= ImGui::IsItemDeactivatedAfterEdit();

      if (mCascadedShadowMapAction && sendAction) {
        mCascadedShadowMapAction->setNewComponent(component);
        actionExecutor.execute(std::move(mCascadedShadowMapAction));
      }
    }
  }

  if (shouldDelete("Directional light")) {
    actionExecutor.execute<EntityDeleteDirectionalLight>(mSelectedEntity);
  }
}

void EntityPanel::renderPointLight(Scene &scene,
                                   ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<PointLight>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Lightbulb) + "  Point light";

  if (auto _ = widgets::Section(SectionName.c_str())) {

    auto &component = scene.entityDatabase.get<PointLight>(mSelectedEntity);

    bool sendAction = false;

    glm::vec4 color = component.color;
    if (widgets::InputColor("Color", color)) {
      if (!mPointLightAction) {
        mPointLightAction =
            std::make_unique<EntitySetPointLight>(mSelectedEntity, component);
      }

      component.color = color;
    }

    sendAction |= ImGui::IsItemDeactivatedAfterEdit();

    float intensity = component.intensity;
    if (widgets::Input("Intensity (in candelas)", intensity, false)) {
      mPointLightAction =
          std::make_unique<EntitySetPointLight>(mSelectedEntity, component);

      component.intensity = intensity;

      sendAction = true;
    }

    float range = component.range;
    if (widgets::Input("Range", range, false)) {
      if (!mPointLightAction) {
        mPointLightAction =
            std::make_unique<EntitySetPointLight>(mSelectedEntity, component);
      }
      component.range = range;

      sendAction = true;
    }

    if (mPointLightAction && sendAction) {
      mPointLightAction->setNewComponent(component);
      actionExecutor.execute(std::move(mPointLightAction));
    }
  }

  if (shouldDelete("Point light")) {
    actionExecutor.execute<EntityDeletePointLight>(mSelectedEntity);
  }
}

void EntityPanel::renderCamera(WorkspaceState &state, Scene &scene,
                               ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<PerspectiveLens>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Video) + " Perpective camera";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component =
        scene.entityDatabase.get<PerspectiveLens>(mSelectedEntity);

    bool sendAction = false;

    float near = component.near;
    if (widgets::Input("Near", near, false)) {
      if (near < 0.0f) {
        near = 0.0f;
      }

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }

      component.near = near;

      sendAction = true;
    }

    float far = component.far;
    if (widgets::Input("Far", far, false)) {
      if (far < 0.0f) {
        far = 0.0f;
      }

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.far = far;

      sendAction = true;
    }

    glm::vec2 sensorSize = component.sensorSize;
    if (widgets::Input("Sensor size (mm)", sensorSize, false)) {
      sensorSize = glm::max(sensorSize, glm::vec2(0.0f));

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.sensorSize = sensorSize;

      sendAction = true;
    }

    float focalLength = component.focalLength;
    if (widgets::Input("Focal length (mm)", focalLength, false)) {
      focalLength = std::max(focalLength, 0.0f);

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.focalLength = focalLength;

      sendAction = true;
    }

    float aperture = component.aperture;
    if (widgets::Input("Aperture", aperture, false)) {
      if (aperture < 0.0f) {
        aperture = 0.0f;
      }

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.aperture = aperture;

      sendAction = true;
    }

    float shutterSpeed = 1.0f / component.shutterSpeed;
    if (widgets::Input("Shutter speed (1/s)", shutterSpeed, false)) {
      if (shutterSpeed < 0.0f) {
        shutterSpeed = 0.0f;
      }

      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.shutterSpeed = 1.0f / shutterSpeed;

      sendAction = true;
    }

    uint32_t sensitivity = component.sensitivity;
    if (widgets::Input("Sensitivity (ISO)", sensitivity, false)) {
      if (!mPerspectiveLensAction) {
        mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
            mSelectedEntity, component);
      }
      component.sensitivity = sensitivity;

      sendAction = true;
    }

    if (sendAction && mPerspectiveLensAction) {
      mPerspectiveLensAction->setNewComponent(component);
      actionExecutor.execute(std::move(mPerspectiveLensAction));
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
        actionExecutor.execute<EntitySetCameraAutoAspectRatio>(mSelectedEntity);
      }

      if (ImGui::Selectable("Custom")) {
        actionExecutor.execute<EntitySetCameraCustomAspectRatio>(
            mSelectedEntity);
      }

      ImGui::EndCombo();
    }

    if (!hasViewportAspectRatio) {
      ImGui::Text("Custom aspect ratio");
      float aspectRatio = component.aspectRatio;
      if (ImGui::DragFloat("###CustomAspectRatio", &aspectRatio,
                           MinCustomAspectRatio, MinCustomAspectRatio,
                           MaxCustomAspectRatio, "%.2f")) {

        if (!mPerspectiveLensAction) {
          mPerspectiveLensAction = std::make_unique<EntitySetPerspectiveLens>(
              mSelectedEntity, component);
        }

        component.aspectRatio = aspectRatio;
      }

      if (ImGui::IsItemDeactivatedAfterEdit() && mPerspectiveLensAction) {
        mPerspectiveLensAction->setNewComponent(component);
        actionExecutor.execute(std::move(mPerspectiveLensAction));
      }
    }

    if (scene.activeCamera != mSelectedEntity) {
      if (ImGui::Button("Set as starting camera")) {
        actionExecutor.execute<SceneSetStartingCamera>(mSelectedEntity);
      }
    } else {
      ImGui::Text("Is the starting camera");
    }
  }

  if (shouldDelete("Perspective camera")) {
    actionExecutor.execute<EntityDeletePerspectiveLens>(mSelectedEntity);
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

    auto localPosition = component.localPosition;
    if (widgets::Input("Position", localPosition, false)) {
      if (!mLocalTransformAction) {
        mLocalTransformAction =
            std::make_unique<EntitySetLocalTransformContinuous>(mSelectedEntity,
                                                                component);
      }

      component.localPosition = localPosition;
    }

    glm::vec3 euler{};
    glm::extractEulerAngleXYZ(glm::toMat4(component.localRotation), euler.x,
                              euler.y, euler.z);
    euler = glm::degrees(euler);

    if (widgets::Input("Rotation", euler, false)) {
      if (!mLocalTransformAction) {
        mLocalTransformAction =
            std::make_unique<EntitySetLocalTransformContinuous>(mSelectedEntity,
                                                                component);
      }

      auto eulerRadians = glm::radians(euler);
      component.localRotation = glm::toQuat(
          glm::eulerAngleXYZ(eulerRadians.x, eulerRadians.y, eulerRadians.z));
    }

    auto localScale = component.localScale;
    if (widgets::Input("Scale", localScale, false)) {
      if (!mLocalTransformAction) {
        mLocalTransformAction =
            std::make_unique<EntitySetLocalTransformContinuous>(mSelectedEntity,
                                                                component);
      }

      component.localScale = localScale;
    }

    if (mLocalTransformAction) {
      mLocalTransformAction->setNewComponent(component);
      actionExecutor.execute(std::move(mLocalTransformAction));
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

void EntityPanel::renderSprite(Scene &scene, AssetRegistry &assetRegistry,
                               ActionExecutor &actionExecutor) {
  static const String SectionName = String(fa::Image) + "  Sprite";

  if (scene.entityDatabase.has<Sprite>(mSelectedEntity)) {

    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle = scene.entityDatabase.get<Sprite>(mSelectedEntity).handle;

      const auto &asset = assetRegistry.getTextures().getAsset(handle);
      static constexpr glm::vec2 TextureSize(80.0f, 80.0f);

      if (auto table = widgets::Table("TableSprite", 2)) {
        table.row("Texture", asset.name);
        table.column("Preview");
        table.column(asset.data.deviceHandle, TextureSize);
      }
    }

    if (shouldDelete("Texture")) {
      actionExecutor.execute<EntityDeleteSprite>(mSelectedEntity);
    }
  }
}

void EntityPanel::renderMesh(Scene &scene, AssetRegistry &assetRegistry,
                             ActionExecutor &actionExecutor) {
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

    if (shouldDelete("Mesh")) {
      actionExecutor.execute<EntityDeleteMesh>(mSelectedEntity);
    }
  }

  if (scene.entityDatabase.has<SkinnedMesh>(mSelectedEntity)) {
    if (auto _ = widgets::Section(SectionName.c_str())) {
      auto handle =
          scene.entityDatabase.get<SkinnedMesh>(mSelectedEntity).handle;

      const auto &asset = assetRegistry.getMeshes().getAsset(handle);

      if (auto table = widgets::Table("TableSkinnedMesh", 2)) {
        table.row("Name", asset.name);
        table.row("Geometries",
                  static_cast<uint32_t>(asset.data.geometries.size()));
      }
    }

    if (shouldDelete("SkinnedMesh")) {
      actionExecutor.execute<EntityDeleteMesh>(mSelectedEntity);
    }
  }
}

void EntityPanel::renderMeshRenderer(Scene &scene, AssetRegistry &assetRegistry,
                                     ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<MeshRenderer>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Desktop) + "  Mesh renderer";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &renderer =
        scene.entityDatabase.get<MeshRenderer>(mSelectedEntity);

    if (auto table = widgets::Table("TableMaterials", 2)) {
      for (size_t i = 0; i < renderer.materials.size(); ++i) {
        const auto &asset =
            assetRegistry.getMaterials().getAsset(renderer.materials.at(i));
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Slot %d", static_cast<uint32_t>(i));
        ImGui::TableNextColumn();
        ImGui::Button(asset.name.c_str());
        if (ImGui::BeginDragDropTarget()) {
          if (auto *payload = ImGui::AcceptDragDropPayload(
                  getAssetTypeString(AssetType::Material).c_str())) {
            auto asset = *static_cast<MaterialAssetHandle *>(payload->Data);
            actionExecutor.execute<EntitySetMeshRendererMaterial>(
                mSelectedEntity, i, asset);
          }
          ImGui::EndDragDropTarget();
        }
      }

      ImGui::TableNextColumn();
      ImGui::Button("Drop a new material slot");
      if (ImGui::BeginDragDropTarget()) {
        if (auto *payload = ImGui::AcceptDragDropPayload(
                getAssetTypeString(AssetType::Material).c_str())) {
          auto asset = *static_cast<MaterialAssetHandle *>(payload->Data);
          actionExecutor.execute<EntityAddMeshRendererMaterialSlot>(
              mSelectedEntity, asset);
        }
        ImGui::EndDragDropTarget();
      }

      if (renderer.materials.size() > 0) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (ImGui::Button("Delete last row")) {
          actionExecutor.execute<EntityRemoveLastMeshRendererMaterialSlot>(
              mSelectedEntity);
        }
      }
    }
  }

  if (shouldDelete("Mesh renderer")) {
    actionExecutor.execute<EntityDeleteMeshRenderer>(mSelectedEntity);
  }
}

void EntityPanel::renderSkinnedMeshRenderer(Scene &scene,
                                            AssetRegistry &assetRegistry,
                                            ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<SkinnedMeshRenderer>(mSelectedEntity)) {
    return;
  }

  static const String SectionName =
      String(fa::Desktop) + " Skinned mesh renderer";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &renderer =
        scene.entityDatabase.get<SkinnedMeshRenderer>(mSelectedEntity);

    if (auto table = widgets::Table("TableMaterials", 2)) {
      for (size_t i = 0; i < renderer.materials.size(); ++i) {
        const auto &asset =
            assetRegistry.getMaterials().getAsset(renderer.materials.at(i));
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Slot %d", static_cast<uint32_t>(i));
        ImGui::TableNextColumn();
        ImGui::Button(asset.name.c_str());
        if (ImGui::BeginDragDropTarget()) {
          if (auto *payload = ImGui::AcceptDragDropPayload(
                  getAssetTypeString(AssetType::Material).c_str())) {
            auto asset = *static_cast<MaterialAssetHandle *>(payload->Data);
            actionExecutor.execute<EntitySetSkinnedMeshRendererMaterial>(
                mSelectedEntity, i, asset);
          }
          ImGui::EndDragDropTarget();
        }
      }

      ImGui::TableNextColumn();
      ImGui::Button("Drop a new material slot");
      if (ImGui::BeginDragDropTarget()) {
        if (auto *payload = ImGui::AcceptDragDropPayload(
                getAssetTypeString(AssetType::Material).c_str())) {
          auto asset = *static_cast<MaterialAssetHandle *>(payload->Data);
          actionExecutor.execute<EntityAddSkinnedMeshRendererMaterialSlot>(
              mSelectedEntity, asset);
        }
        ImGui::EndDragDropTarget();
      }

      if (renderer.materials.size() > 0) {
        ImGui::TableNextColumn();
        if (ImGui::Button("Delete last row")) {
          actionExecutor
              .execute<EntityRemoveLastSkinnedMeshRendererMaterialSlot>(
                  mSelectedEntity);
        }
      }
    }
  }

  if (shouldDelete("Skinned mesh renderer")) {
    actionExecutor.execute<EntityDeleteSkinnedMeshRenderer>(mSelectedEntity);
  }
}

void EntityPanel::renderSkeleton(Scene &scene, AssetRegistry &assetRegistry,
                                 ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Skeleton>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Bone) + "  Skeleton";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    bool showBones = scene.entityDatabase.has<SkeletonDebug>(mSelectedEntity);

    const auto &skeleton = scene.entityDatabase.get<Skeleton>(mSelectedEntity);

    auto handle = skeleton.assetHandle;

    const auto &asset = assetRegistry.getSkeletons().getAsset(handle);

    if (auto table = widgets::Table("TableSkinnedMesh", 2)) {
      table.row("Name", asset.name);
      table.row("Number of joints",
                static_cast<uint32_t>(skeleton.jointNames.size()));
    }

    if (ImGui::Checkbox("Show bones", &showBones)) {
      actionExecutor.execute<EntityToggleSkeletonDebugBones>(mSelectedEntity);
    }
  }

  if (shouldDelete("Skeleton")) {
    actionExecutor.execute<EntityDeleteSkeleton>(mSelectedEntity);
  }
}

void EntityPanel::renderJointAttachment(Scene &scene,
                                        ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<JointAttachment>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Bone) + " Joint attachment";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    if (!scene.entityDatabase.has<Parent>(mSelectedEntity) ||
        !scene.entityDatabase.has<Skeleton>(
            scene.entityDatabase.get<Parent>(mSelectedEntity).parent)) {
      ImGui::Text("Entity must be an immediate child of a skeleton");
      return;
    }

    auto parentEntity =
        scene.entityDatabase.get<Parent>(mSelectedEntity).parent;

    const auto &skeleton = scene.entityDatabase.get<Skeleton>(parentEntity);

    auto &attachment =
        scene.entityDatabase.get<JointAttachment>(mSelectedEntity);

    auto label = attachment.joint >= 0 &&
                         attachment.joint <
                             static_cast<int16_t>(skeleton.jointNames.size())
                     ? skeleton.jointNames.at(attachment.joint)
                     : "Select joint";

    if (ImGui::Button(label.c_str())) {
      ImGui::OpenPopup("SetJointAttachment");
    }

    if (ImGui::BeginPopup("SetJointAttachment")) {
      for (size_t i = 0; i < skeleton.jointNames.size(); ++i) {
        if (ImGui::Selectable(skeleton.jointNames.at(i).c_str())) {
          auto newAttachment = attachment;
          newAttachment.joint = static_cast<int16_t>(i);

          actionExecutor.execute<EntityDefaultUpdateComponent<JointAttachment>>(
              mSelectedEntity, attachment, newAttachment);
        }
      }

      ImGui::EndPopup();
    }
  }

  if (shouldDelete("Joint attachment")) {
    actionExecutor.execute<EntityDefaultDeleteAction<JointAttachment>>(
        mSelectedEntity);
  }
}

void EntityPanel::renderAnimation(WorkspaceState &state, Scene &scene,
                                  AssetRegistry &assetRegistry,
                                  ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<Animator>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Circle) + " Animator";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    auto &component = scene.entityDatabase.get<Animator>(mSelectedEntity);
    const auto &animatorAsset =
        assetRegistry.getAnimators().getAsset(component.asset).data;

    auto currentStateIndex =
        component.currentState < animatorAsset.states.size()
            ? component.currentState
            : animatorAsset.initialState;

    const auto &currentState = animatorAsset.states.at(currentStateIndex);

    bool isSimulation = state.mode == WorkspaceMode::Simulation;

    ImGui::Text("Current state: %s", currentState.name.c_str());
    if (auto table = widgets::Table("Transitions", isSimulation ? 3 : 2)) {
      table.row("Event", "Target");
      for (auto &transition : currentState.transitions) {
        table.row(transition.eventName,
                  animatorAsset.states.at(transition.target).name);

        if (isSimulation) {
          ImGui::TableNextColumn();
          ImGui::PushID(transition.eventName.c_str());
          if (ImGui::Button("Toggle")) {
            scene.entityDatabase.set<AnimatorEvent>(mSelectedEntity,
                                                    {transition.eventName});
          }
          ImGui::PopID();
        }
      }
    }

    if (state.mode == WorkspaceMode::Simulation) {
      if (assetRegistry.getAnimations().hasAsset(currentState.animation)) {
        const auto &animationAsset =
            assetRegistry.getAnimations().getAsset(currentState.animation).data;

        ImGui::Text("Time");
        float animationTime = component.normalizedTime * animationAsset.time;
        if (ImGui::SliderFloat("###AnimationTime", &animationTime, 0.0f,
                               animationAsset.time)) {
          component.normalizedTime = animationTime / animationAsset.time;
        }

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

  if (shouldDelete("Animator")) {
    actionExecutor.execute<EntityDeleteAnimator>(mSelectedEntity);
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
          actionExecutor.execute<EntitySetCollidableType>(mSelectedEntity,
                                                          type);
        }
      }
      ImGui::EndCombo();
    }

    bool sendAction = false;

    if (collidable.geometryDesc.type == PhysicsGeometryType::Box) {
      auto &box = std::get<PhysicsGeometryBox>(collidable.geometryDesc.params);
      auto halfExtents = box.halfExtents;

      if (widgets::Input("Half extents", halfExtents, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        box.halfExtents = halfExtents;
      }
    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Sphere) {
      auto &sphere =
          std::get<PhysicsGeometrySphere>(collidable.geometryDesc.params);
      float radius = sphere.radius;

      if (widgets::Input("Radius", radius, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        sphere.radius = radius;
      }
    } else if (collidable.geometryDesc.type == PhysicsGeometryType::Capsule) {
      auto &capsule =
          std::get<PhysicsGeometryCapsule>(collidable.geometryDesc.params);
      float radius = capsule.radius;
      float halfHeight = capsule.halfHeight;

      if (widgets::Input("Radius", radius, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        capsule.radius = radius;
      }

      if (widgets::Input("Half height", halfHeight, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        capsule.halfHeight = halfHeight;
      }
    }

    {
      auto &material = collidable.materialDesc;

      float dynamicFriction = material.dynamicFriction;

      if (widgets::Input("Dynamic friction", dynamicFriction, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        material.dynamicFriction = dynamicFriction;
      }

      float restitution = material.restitution;
      if (widgets::Input("Restitution", restitution, false)) {
        if (restitution > 1.0f) {
          restitution = 1.0f;
        }

        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        material.restitution = restitution;
      }

      float staticFriction = material.staticFriction;
      if (widgets::Input("Static friction", staticFriction, false)) {
        if (!mCollidableAction) {
          mCollidableAction = std::make_unique<EntitySetCollidable>(
              mSelectedEntity, collidable);
        }

        material.staticFriction = staticFriction;
      }
    }

    if (mCollidableAction) {
      mCollidableAction->setNewComponent(collidable);

      actionExecutor.execute(std::move(mCollidableAction));
    }
  }

  if (shouldDelete("Collidable")) {
    actionExecutor.execute<EntityDeleteCollidable>(mSelectedEntity);
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
      if (!mRigidBodyAction) {
        mRigidBodyAction =
            std::make_unique<EntitySetRigidBody>(mSelectedEntity, rigidBody);
      }

      rigidBody.dynamicDesc.mass = mass;
    }

    glm::vec3 inertia = rigidBody.dynamicDesc.inertia;
    if (widgets::Input("Inertia", inertia, false)) {
      if (!mRigidBodyAction) {
        mRigidBodyAction =
            std::make_unique<EntitySetRigidBody>(mSelectedEntity, rigidBody);
      }

      rigidBody.dynamicDesc.inertia = inertia;
    }

    ImGui::Text("Apply gravity");
    bool applyGravity = rigidBody.dynamicDesc.applyGravity;
    if (ImGui::Checkbox("Apply gravity###ApplyGravity", &applyGravity)) {
      if (!mRigidBodyAction) {
        mRigidBodyAction =
            std::make_unique<EntitySetRigidBody>(mSelectedEntity, rigidBody);
      }

      rigidBody.dynamicDesc.applyGravity = applyGravity;
    }

    if (mRigidBodyAction) {
      mRigidBodyAction->setNewComponent(rigidBody);
      actionExecutor.execute(std::move(mRigidBodyAction));
    }
  }

  if (shouldDelete("RigidBody")) {
    actionExecutor.execute<EntityDeleteRigidBody>(mSelectedEntity);
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
      if (!mTextAction) {
        mTextAction = std::make_unique<EntitySetText>(mSelectedEntity, text);
      }

      text.text = tmpText;
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
      sendAction = true;
    }

    float lineHeight = text.lineHeight;
    if (widgets::Input("Line height", lineHeight, false)) {
      if (!mTextAction) {
        mTextAction = std::make_unique<EntitySetText>(mSelectedEntity, text);
      }

      text.lineHeight = lineHeight;
    }

    ImGui::Text("Select font");
    if (ImGui::BeginCombo("###SelectFont", fonts.at(text.font).name.c_str(),
                          0)) {
      for (const auto &[handle, data] : fonts) {
        bool selectable = handle == text.font;

        const auto &fontName = data.name;

        if (ImGui::Selectable(fontName.c_str(), &selectable)) {
          if (!mTextAction) {
            mTextAction =
                std::make_unique<EntitySetText>(mSelectedEntity, text);
          }

          text.font = handle;
        }
      }
      ImGui::EndCombo();
    }

    if (mTextAction) {
      mTextAction->setNewComponent(text);

      actionExecutor.execute(std::move(mTextAction));
    }
  }

  if (shouldDelete("Text")) {
    actionExecutor.execute<EntityDeleteText>(mSelectedEntity);
  }
}

void EntityPanel::renderAudio(Scene &scene, AssetRegistry &assetRegistry,
                              ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<AudioSource>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Music) + "  Audio";

  if (auto _ = widgets::Section(SectionName.c_str())) {
    const auto &audio = scene.entityDatabase.get<AudioSource>(mSelectedEntity);
    const auto &asset = assetRegistry.getAudios().getAsset(audio.source);

    ImGui::Text("Name: %s", asset.name.c_str());
  }

  if (shouldDelete("Audio")) {
    actionExecutor.execute<EntityDeleteAudio>(mSelectedEntity);
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
          } else if (variable.type == LuaScriptVariableType::AssetTexture) {
            type = "Texture";
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
          } else if (script.variables.at(name).isType(
                         LuaScriptVariableType::AssetTexture)) {
            auto handle = script.variables.at(name).get<TextureAssetHandle>();
            value = assetRegistry.getTextures().getAsset(handle).name;
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
                    : PrefabAssetHandle::Null;

            const auto width = ImGui::GetWindowContentRegionWidth();
            const float halfWidth = width * 0.5f;
            if (value == PrefabAssetHandle::Null) {
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
          } else if (variable.type == LuaScriptVariableType::AssetTexture) {
            ImGui::Text("%s", name.c_str());
            auto value =
                existingVariable.isType(LuaScriptVariableType::AssetTexture)
                    ? existingVariable.get<TextureAssetHandle>()
                    : TextureAssetHandle::Null;

            const auto width = ImGui::GetWindowContentRegionWidth();
            const float halfWidth = width * 0.5f;
            if (value == TextureAssetHandle::Null) {
              ImGui::Button("Drag texture here", ImVec2(width, halfWidth));
            } else {
              String buttonLabel =
                  "Replace current texture: " +
                  assetRegistry.getTextures().getAsset(value).name;
              ImGui::Button(buttonLabel.c_str(), ImVec2(width, halfWidth));
            }

            if (ImGui::BeginDragDropTarget()) {
              if (auto *payload = ImGui::AcceptDragDropPayload(
                      getAssetTypeString(AssetType::Texture).c_str())) {
                auto handle = *static_cast<TextureAssetHandle *>(payload->Data);
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

  if (shouldDelete("Scripting")) {
    actionExecutor.execute<EntityDeleteScript>(mSelectedEntity);
  }
}

void EntityPanel::renderSkybox(Scene &scene, AssetRegistry &assetRegistry,
                               ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<EnvironmentSkybox>(mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Cloud) + "  Skybox";

  if (auto section = widgets::Section(SectionName.c_str())) {
    float width = section.getClipRect().GetWidth();
    const float height = width * 0.5f;

    auto &skybox = scene.entityDatabase.get<EnvironmentSkybox>(mSelectedEntity);

    ImGui::Text("Type");
    if (ImGui::BeginCombo("###SkyboxType",
                          getSkyboxTypeLabel(skybox.type).c_str())) {
      if (ImGui::Selectable("Color")) {
        auto newSkybox = skybox;
        newSkybox.type = EnvironmentSkyboxType::Color;

        actionExecutor.execute<EntityDefaultUpdateComponent<EnvironmentSkybox>>(
            mSelectedEntity, skybox, newSkybox);
      } else if (ImGui::Selectable("Texture")) {
        auto newSkybox = skybox;
        newSkybox.type = EnvironmentSkyboxType::Texture;

        actionExecutor.execute<EntityDefaultUpdateComponent<EnvironmentSkybox>>(
            mSelectedEntity, skybox, newSkybox);
      }

      ImGui::EndCombo();
    }

    if (skybox.type == EnvironmentSkyboxType::Color) {
      bool sendAction = false;

      glm::vec4 color = skybox.color;
      if (widgets::InputColor("Color", color)) {
        if (!mEnvironmentSkyboxAction) {
          mEnvironmentSkyboxAction =
              std::make_unique<EntityDefaultUpdateComponent<EnvironmentSkybox>>(
                  mSelectedEntity, skybox);
        }

        skybox.color = color;
      }

      sendAction |= ImGui::IsItemDeactivatedAfterEdit();

      if (sendAction && mEnvironmentSkyboxAction) {
        mEnvironmentSkyboxAction->setNewComponent(skybox);
        actionExecutor.execute(std::move(mEnvironmentSkyboxAction));
      }

    } else if (skybox.type == EnvironmentSkyboxType::Texture) {
      if (assetRegistry.getEnvironments().hasAsset(skybox.texture)) {
        const auto &envAsset =
            assetRegistry.getEnvironments().getAsset(skybox.texture);

        imgui::image(envAsset.preview, ImVec2(width, height), ImVec2(0, 0),
                     ImVec2(1, 1), ImGui::GetID("environment-texture-drop"));

        dndEnvironmentAsset(section, mSelectedEntity, skybox, actionExecutor);

        if (ImGui::Button(fa::Times)) {
          auto newSkybox = skybox;
          newSkybox.texture = EnvironmentAssetHandle::Null;
          actionExecutor
              .execute<EntityDefaultUpdateComponent<EnvironmentSkybox>>(
                  mSelectedEntity, skybox, newSkybox);
        }

      } else {
        ImGui::Button("Drag environment asset here", ImVec2(width, height));
        dndEnvironmentAsset(section, mSelectedEntity, skybox, actionExecutor);
      }
    }

    if (scene.activeEnvironment != mSelectedEntity) {
      if (ImGui::Button("Set as starting environment")) {
        actionExecutor.execute<SceneSetStartingEnvironment>(mSelectedEntity);
      }
    } else {
      ImGui::Text("Is the starting environment");
    }
  }

  if (shouldDelete("Skybox")) {
    actionExecutor.execute<EntityDefaultDeleteAction<EnvironmentSkybox>>(
        mSelectedEntity);
  }
}

void EntityPanel::renderEnvironmentLighting(Scene &scene,
                                            AssetRegistry &assetRegistry,
                                            ActionExecutor &actionExecutor) {
  if (!scene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
          mSelectedEntity)) {
    return;
  }

  static const String SectionName = String(fa::Sun) + "  Environment lighting";

  if (auto section = widgets::Section(SectionName.c_str())) {

    ImGui::Text("Source");
    if (ImGui::BeginCombo("###Source", "Skybox")) {
      ImGui::EndCombo();
    }
  }

  if (shouldDelete("EnvironmentLighting")) {
    actionExecutor
        .execute<EntityDefaultDeleteAction<EnvironmentLightingSkyboxSource>>(
            mSelectedEntity);
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
      actionExecutor.execute<EntitySetLocalTransformContinuous>(
          mSelectedEntity, quoll::LocalTransform{}, quoll::LocalTransform{});
    }

    if (!scene.entityDatabase.has<RigidBody>(mSelectedEntity) &&
        ImGui::Selectable("Rigid body")) {
      actionExecutor.execute<EntityCreateRigidBody>(mSelectedEntity);
    }

    if (!scene.entityDatabase.has<Collidable>(mSelectedEntity) &&
        ImGui::Selectable("Collidable")) {
      actionExecutor.execute<EntityCreateCollidable>(mSelectedEntity);
    }

    if (!scene.entityDatabase.has<MeshRenderer>(mSelectedEntity) &&
        ImGui::Selectable("Mesh renderer")) {
      actionExecutor.execute<EntityCreateMeshRenderer>(mSelectedEntity);
    }

    if (!scene.entityDatabase.has<SkinnedMeshRenderer>(mSelectedEntity) &&
        ImGui::Selectable("Skinned mesh renderer")) {
      actionExecutor.execute<EntityCreateSkinnedMeshRenderer>(mSelectedEntity);
    }

    if (!scene.entityDatabase.has<DirectionalLight>(mSelectedEntity) &&
        !scene.entityDatabase.has<PointLight>(mSelectedEntity)) {
      if (ImGui::Selectable("Directional light")) {
        actionExecutor.execute<EntityCreateDirectionalLight>(mSelectedEntity);
      }

      if (ImGui::Selectable("Point light")) {
        actionExecutor.execute<EntityCreatePointLight>(mSelectedEntity);
      }
    }

    if (!scene.entityDatabase.has<PerspectiveLens>(mSelectedEntity) &&
        ImGui::Selectable("Perspective camera")) {
      actionExecutor.execute<EntityCreatePerspectiveLens>(mSelectedEntity);
    }

    if (!scene.entityDatabase.has<Text>(mSelectedEntity) &&
        ImGui::Selectable("Text")) {
      Text text{"Hello world"};
      text.font = assetRegistry.getDefaultObjects().defaultFont;
      actionExecutor.execute<EntityCreateText>(mSelectedEntity, text);
    }

    if (!scene.entityDatabase.has<JointAttachment>(mSelectedEntity) &&
        ImGui::Selectable("Joint attachment")) {
      actionExecutor.execute<EntityDefaultCreateComponent<JointAttachment>>(
          mSelectedEntity, JointAttachment{});
    }

    if (!scene.entityDatabase.has<EnvironmentSkybox>(mSelectedEntity) &&
        ImGui::Selectable("Skybox")) {
      actionExecutor.execute<EntityDefaultCreateComponent<EnvironmentSkybox>>(
          mSelectedEntity, EnvironmentSkybox{});
    }

    if (!scene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
            mSelectedEntity) &&
        ImGui::Selectable("Environment lighting")) {
      actionExecutor.execute<
          EntityDefaultCreateComponent<EnvironmentLightingSkyboxSource>>(
          mSelectedEntity, EnvironmentLightingSkyboxSource{});
    }

    ImGui::EndPopup();
  }
}

void EntityPanel::handleDragAndDrop(Scene &scene, AssetRegistry &assetRegistry,
                                    ActionExecutor &actionExecutor) {
  const auto width = ImGui::GetWindowContentRegionWidth();
  const float halfWidth = width * 0.5f;

  ImGui::Button("Drag asset here", ImVec2(width, halfWidth));

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Mesh).c_str())) {
      auto asset = *static_cast<MeshAssetHandle *>(payload->Data);
      actionExecutor.execute<EntitySetMesh>(mSelectedEntity, asset);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::SkinnedMesh).c_str())) {
      auto asset = *static_cast<MeshAssetHandle *>(payload->Data);
      actionExecutor.execute<EntitySetMesh>(mSelectedEntity, asset);
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Audio).c_str())) {
      auto asset = *static_cast<AudioAssetHandle *>(payload->Data);

      if (scene.entityDatabase.has<AudioSource>(mSelectedEntity)) {
        actionExecutor.execute<EntitySetAudio>(mSelectedEntity, asset);
      } else {
        actionExecutor.execute<EntityCreateAudio>(mSelectedEntity, asset);
      }
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::LuaScript).c_str())) {
      auto asset = *static_cast<LuaScriptAssetHandle *>(payload->Data);

      if (scene.entityDatabase.has<Script>(mSelectedEntity)) {
        actionExecutor.execute<EntitySetScript>(mSelectedEntity, asset);
      } else {
        actionExecutor.execute<EntityCreateScript>(mSelectedEntity, asset);
      }
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Animator).c_str())) {
      auto asset = *static_cast<AnimatorAssetHandle *>(payload->Data);

      if (scene.entityDatabase.has<Animator>(mSelectedEntity)) {
        actionExecutor.execute<EntitySetAnimator>(mSelectedEntity, asset);
      } else {
        actionExecutor.execute<EntityCreateAnimator>(mSelectedEntity, asset);
      }
    }

    if (auto *payload = ImGui::AcceptDragDropPayload(
            getAssetTypeString(AssetType::Texture).c_str())) {
      auto asset = *static_cast<TextureAssetHandle *>(payload->Data);

      if (scene.entityDatabase.has<Sprite>(mSelectedEntity)) {
        actionExecutor.execute<EntitySetSprite>(mSelectedEntity, asset);
      } else {
        actionExecutor.execute<EntityCreateSprite>(mSelectedEntity, asset);
      }
    }

    ImGui::EndDragDropTarget();
  }
}

bool EntityPanel::shouldDelete(const char *component) {
  bool clicked = false;
  if (ImGui::BeginPopupContextItem(component,
                                   ImGuiPopupFlags_MouseButtonRight)) {
    if (ImGui::MenuItem("Delete")) {
      clicked = true;
    }

    ImGui::EndPopup();
  }

  return clicked;
}

} // namespace quoll::editor
