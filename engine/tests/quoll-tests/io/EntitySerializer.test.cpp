#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/input/InputMap.h"
#include "quoll/io/EntitySerializer.h"
#include "quoll/lua-scripting/LuaScript.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Torque.h"
#include "quoll/physx/PhysxInstance.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMeshRenderer.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/Sprite.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/JointAttachment.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/text/Text.h"
#include "quoll/ui/UICanvas.h"
#include "quoll/ui/UICanvasRenderRequest.h"
#include "quoll-tests/Testing.h"

class EntitySerializerTest : public ::testing::Test {
public:
  EntitySerializerTest()
      : entitySerializer(assetCache.getRegistry(), entityDatabase),
        assetCache("/") {}

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    quoll::AssetData<TAssetData> info{};
    info.type = quoll::AssetCache::getAssetType<TAssetData>();
    info.uuid = quoll::Uuid::generate();
    info.data = data;

    assetCache.getRegistry().add(info);

    return assetCache.request<TAssetData>(info.uuid).data();
  }

public:
  quoll::AssetCache assetCache;
  quoll::EntityDatabase entityDatabase;
  quoll::detail::EntitySerializer entitySerializer;
};

// Name
TEST_F(EntitySerializerTest,
       SetsNameToDefaultNameWithoutIdComponentIfNameIsEmpty) {
  {
    auto entity = entityDatabase.create();
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<quoll::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  }

  {
    auto entity = entityDatabase.create();
    entityDatabase.set<quoll::Name>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<quoll::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  }
}

TEST_F(EntitySerializerTest,
       SetsNameToDefaultNameWithIdComponentIfNameIsEmpty) {
  {
    auto entity = entityDatabase.create();
    entityDatabase.set<quoll::Id>(entity, {15});
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<quoll::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  }

  {
    auto entity = entityDatabase.create();
    entityDatabase.set<quoll::Id>(entity, {15});
    entityDatabase.set<quoll::Name>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<quoll::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  }
}

TEST_F(EntitySerializerTest, CreatesNameFieldUsingNameComponentIfExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Test entity"});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["name"]);
  EXPECT_EQ(node["name"].as<quoll::String>(""), "Test entity");
}

// Transform
TEST_F(EntitySerializerTest,
       CreatesTransformFieldWithDefaultsIfTransformComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(entityDatabase.has<quoll::LocalTransform>(entity));
  const auto &defaults = entityDatabase.get<quoll::LocalTransform>(entity);

  EXPECT_TRUE(node["transform"]);
  EXPECT_EQ(node["transform"]["position"].as<glm::vec3>(glm::vec3(5.0f)),
            defaults.localPosition);
  EXPECT_EQ(node["transform"]["rotation"].as<glm::quat>(
                glm::quat(0.0f, 0.0f, 0.0f, 0.0f)),
            defaults.localRotation);
  EXPECT_EQ(node["transform"]["scale"].as<glm::vec3>(glm::vec3(5.0f)),
            defaults.localScale);
}

TEST_F(EntitySerializerTest,
       CreatesTransformFieldFromTransformComponentIfExists) {
  auto entity = entityDatabase.create();

  quoll::LocalTransform transform{};
  transform.localPosition = glm::vec3{2.0f};
  transform.localRotation = glm::quat{0.5f, 0.5f, 0.5f, 0.5f};
  transform.localScale = glm::vec3{0.2f};

  entityDatabase.set(entity, transform);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["transform"]);
  EXPECT_EQ(node["transform"]["position"].as<glm::vec3>(glm::vec3(0.0f)),
            transform.localPosition);
  EXPECT_EQ(node["transform"]["rotation"].as<glm::quat>(
                glm::quat(0.0f, 0.0f, 0.0f, 0.0f)),
            transform.localRotation);
  EXPECT_EQ(node["transform"]["scale"].as<glm::vec3>(glm::vec3(0.0f)),
            transform.localScale);
}

// Sprite
TEST_F(EntitySerializerTest,
       DoesNotCreateSpriteFieldIfMeshComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["sprite"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateSpriteFieldIfTextureAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Sprite>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["sprite"]);
}

TEST_F(EntitySerializerTest, CreatesSpriteFieldIfTextureAssetIsInRegistry) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Sprite>(entity, {texture});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["sprite"]);
  EXPECT_EQ(node["sprite"].as<quoll::Uuid>(), texture.meta().uuid);
}

// Mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshFieldIfMeshComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateMeshFieldIfMeshAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Mesh>(entity,
                                  {quoll::AssetRef<quoll::MeshAsset>()});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesMeshFieldIfMeshAssetIsInRegistry) {
  auto mesh = createAsset<quoll::MeshAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Mesh>(entity, {mesh});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<quoll::Uuid>(quoll::Uuid{}), mesh.meta().uuid);
}

// Mesh renderer
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshRendererFieldIfMeshRendererComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["meshRenderer"]);
}

TEST_F(EntitySerializerTest, CreatesMeshRendererFieldWithMaterials) {
  auto material1 = createAsset<quoll::MaterialAsset>();
  auto material2 = createAsset<quoll::MaterialAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(entity, {{material1, material2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<quoll::Uuid>(quoll::Uuid{}),
            material1.meta().uuid);
  EXPECT_EQ(node["meshRenderer"]["materials"][1].as<quoll::Uuid>(quoll::Uuid{}),
            material2.meta().uuid);
}

TEST_F(EntitySerializerTest,
       CreatesMeshRendererAndIgnoresNonExistentMaterials) {

  auto material1 = createAsset<quoll::MaterialAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(
      entity, {{material1, quoll::AssetRef<quoll::MaterialAsset>()}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<quoll::Uuid>(quoll::Uuid{}),
            material1.meta().uuid);
}

TEST_F(EntitySerializerTest, CreatesMeshRendererWithNoMaterials) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 0);
}

// Skinned mesh renderer
TEST_F(
    EntitySerializerTest,
    DoesNotCreateSkinnedMeshRendererFieldIfSkinnedMeshRendererComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMeshRenderer"]);
}

TEST_F(EntitySerializerTest, CreatesSkinnedMeshRendererFieldWithMaterials) {
  auto material1 = createAsset<quoll::MaterialAsset>();
  auto material2 = createAsset<quoll::MaterialAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMeshRenderer>(entity,
                                                 {{material1, material2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<quoll::Uuid>(
                quoll::Uuid{}),
            material1.meta().uuid);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][1].as<quoll::Uuid>(
                quoll::Uuid{}),
            material2.meta().uuid);
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshRendererAndIgnoresNonExistentMaterials) {
  auto material1 = createAsset<quoll::MaterialAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMeshRenderer>(
      entity, {{material1, quoll::AssetRef<quoll::MaterialAsset>()}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<quoll::Uuid>(
                quoll::Uuid{}),
            material1.meta().uuid);
}

TEST_F(EntitySerializerTest, CreatesSkinnedMeshRendererWithNoMaterials) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMeshRenderer>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 0);
}

// Skeleton
TEST_F(EntitySerializerTest,
       DoesNotCreateSkeletonFieldIfSkeletonComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateSkeletonFieldIfSkeletonAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  quoll::Skeleton component{};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest, CreatesSkeletonFieldIfSkeletonAssetIsInRegistry) {
  auto skeleton = createAsset<quoll::SkeletonAsset>();

  auto entity = entityDatabase.create();
  quoll::Skeleton component{};
  component.assetHandle = skeleton;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skeleton"]);
  EXPECT_EQ(node["skeleton"].as<quoll::Uuid>(quoll::Uuid{}),
            skeleton.meta().uuid);
}

// Joint attachment
TEST_F(
    EntitySerializerTest,
    DoesNotCreateJointAttachmentFieldIfJointAttachmentComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["jointAttachment"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateJointAttachmentFieldIfJointAttachmentIdIsNonZero) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::JointAttachment>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["jointAttachment"]);
}

TEST_F(EntitySerializerTest, CreatesJointAttachmentFieldWithJointId) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::JointAttachment>(entity, {10});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["jointAttachment"]);
  EXPECT_EQ(node["jointAttachment"]["joint"].as<u32>(0), 10);
}

// Animator
TEST_F(EntitySerializerTest,
       DoesNotCreateAnimatorFieldIfAnimatorAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  quoll::Animator component{};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["animator"]);
}

TEST_F(EntitySerializerTest, CreatesAnimatorWithValidAnimations) {
  auto animator = createAsset<quoll::AnimatorAsset>();

  quoll::Animator component{};
  component.asset = animator;
  component.currentState = 0;

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["animator"]);
  EXPECT_EQ(node["animator"]["asset"].as<quoll::Uuid>(quoll::Uuid{}),
            animator.meta().uuid);
}

// Directional light
TEST_F(EntitySerializerTest,
       DoesNotCreateLightFieldIfNoDirectionalLightComponent) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["light"]);
}

TEST_F(EntitySerializerTest,
       CreatesLightFieldIfDirectionalLightComponentExists) {
  auto entity = entityDatabase.create();

  quoll::DirectionalLight light{};
  light.intensity = 5.5f;
  light.color = glm::vec4{0.5f};
  entityDatabase.set(entity, light);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["light"]);
  EXPECT_TRUE(node["light"].IsMap());
  EXPECT_EQ(node["light"]["type"].as<u32>(1000), 0);
  EXPECT_EQ(node["light"]["intensity"].as<f32>(-1.0f), light.intensity);
  EXPECT_EQ(node["light"]["color"].as<glm::vec4>(glm::vec4{-1.0f}),
            light.color);
  EXPECT_FALSE(node["light"]["direction"]);
}

// Directional light - Cascaded shadow map
TEST_F(EntitySerializerTest,
       DoesNotCreateShadowFieldInLightIfNoDirectionalLightComponent) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::CascadedShadowMap>(entity, {});
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["light"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateShadowFieldInLightIfNoCascadedShadowComponent) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::DirectionalLight>(entity, {});
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["light"]);
  EXPECT_FALSE(node["light"]["shadow"]);
}

TEST_F(
    EntitySerializerTest,
    CreatesShadowFieldInLightIfDirectionalLightComponentAndCascadedShadowMapComponentsExist) {
  auto entity = entityDatabase.create();

  quoll::DirectionalLight light{};
  entityDatabase.set(entity, light);

  quoll::CascadedShadowMap shadow{};
  shadow.softShadows = false;
  shadow.splitLambda = 0.2f;
  shadow.numCascades = 4;
  entityDatabase.set(entity, shadow);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["light"]);
  EXPECT_TRUE(node["light"]["shadow"]);
  EXPECT_TRUE(node["light"]["shadow"].IsMap());
  EXPECT_EQ(node["light"]["shadow"]["softShadows"].as<bool>(true),
            shadow.softShadows);
  EXPECT_EQ(node["light"]["shadow"]["splitLambda"].as<f32>(1.0f),
            shadow.splitLambda);
  EXPECT_EQ(node["light"]["shadow"]["numCascades"].as<u32>(0),
            shadow.numCascades);
}

// Point light
TEST_F(EntitySerializerTest, DoesNotCreateLightFieldIfNoPointLight) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["light"]);
}

TEST_F(EntitySerializerTest, CreatesLightFieldIfPointLightComponentExists) {
  auto entity = entityDatabase.create();

  quoll::PointLight light{};
  light.intensity = 5.5f;
  light.color = glm::vec4{0.5f};
  light.range = 25.0f;
  entityDatabase.set(entity, light);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["light"]);
  EXPECT_TRUE(node["light"].IsMap());
  EXPECT_EQ(node["light"]["type"].as<u32>(1000), 1);
  EXPECT_EQ(node["light"]["intensity"].as<f32>(-1.0f), light.intensity);
  EXPECT_EQ(node["light"]["color"].as<glm::vec4>(glm::vec4{-1.0f}),
            light.color);
  EXPECT_EQ(node["light"]["range"].as<f32>(-1.0f), light.range);
}

// Camera
TEST_F(EntitySerializerTest,
       DoesNotCreateCameraFieldIfLensComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["camera"]);
}

TEST_F(EntitySerializerTest, CreatesCameraFieldIfLensComponentExists) {
  auto entity = entityDatabase.create();

  quoll::PerspectiveLens lens{};
  lens.aspectRatio = 2.5f;
  lens.far = 200.0f;
  lens.near = 0.2f;
  lens.sensorSize = glm::vec2(50.0f, 45.0f);
  lens.focalLength = 100.0f;
  lens.aperture = 1.5f;
  lens.shutterSpeed = 2.5f;
  lens.sensitivity = 2500;
  entityDatabase.set(entity, lens);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["camera"]);
  EXPECT_TRUE(node["camera"].IsMap());
  EXPECT_EQ(node["camera"]["type"].as<u32>(1000), 0);
  EXPECT_EQ(node["camera"]["near"].as<f32>(-1.0f), lens.near);
  EXPECT_EQ(node["camera"]["far"].as<f32>(-1.0f), lens.far);
  EXPECT_EQ(node["camera"]["aspectRatio"].as<f32>(-1.0f), lens.aspectRatio);
  EXPECT_EQ(node["camera"]["sensorSize"].as<glm::vec2>(glm::vec2{-1.0f, -1.0f}),
            lens.sensorSize);
  EXPECT_EQ(node["camera"]["focalLength"].as<f32>(-1.0f), lens.focalLength);
  EXPECT_EQ(node["camera"]["aperture"].as<f32>(-1.0f), lens.aperture);
  EXPECT_EQ(node["camera"]["shutterSpeed"].as<f32>(-1.0f), lens.shutterSpeed);
  EXPECT_EQ(node["camera"]["sensitivity"].as<f32>(-1.0f), lens.sensitivity);
}

TEST_F(EntitySerializerTest,
       SetsCameraAspectRatioToAutoIfAutoAspectRatioComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  entityDatabase.set<quoll::AutoAspectRatio>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["camera"]);
  EXPECT_TRUE(node["camera"].IsMap());
  EXPECT_EQ(node["camera"]["aspectRatio"].as<quoll::String>(""), "auto");
}

// Audio
TEST_F(EntitySerializerTest,
       DoesNotCreateAudioFieldIfAudioComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateAudioFieldIfAudioAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest, CreatesAudioFieldIfAudioAssetIsInRegistry) {
  auto audio = createAsset<quoll::AudioAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {audio});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["audio"]);
  EXPECT_TRUE(node["audio"].IsMap());
  EXPECT_EQ(node["audio"]["source"].as<quoll::Uuid>(quoll::Uuid{}),
            audio.meta().uuid);
}

// Script
TEST_F(EntitySerializerTest,
       DoesNotCreateScriptFieldIfScriptComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateScriptFieldIfScriptAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LuaScript>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest, CreatesScriptFieldIfScriptAssetIsRegistry) {
  std::unordered_map<quoll::String, quoll::LuaScriptVariable> variables{
      {"test_str",
       quoll::LuaScriptVariable{quoll::LuaScriptVariableType::String}},
      {"test_prefab",
       quoll::LuaScriptVariable{quoll::LuaScriptVariableType::AssetPrefab}},
      {"test_texture",
       quoll::LuaScriptVariable{quoll::LuaScriptVariableType::AssetTexture}}};

  auto script = createAsset<quoll::LuaScriptAsset>({.variables = variables});
  auto prefab = createAsset<quoll::PrefabAsset>();
  auto texture = createAsset<quoll::TextureAsset>();

  auto entity = entityDatabase.create();

  quoll::LuaScript component{script};
  component.variables.insert_or_assign("test_str",
                                       quoll::String("hello world"));
  component.variables.insert_or_assign("test_str_invalid",
                                       quoll::String("hello world"));
  component.variables.insert_or_assign("test_prefab", prefab);
  component.variables.insert_or_assign("test_texture", texture);
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["script"]);
  EXPECT_EQ(node["script"]["asset"].as<quoll::Uuid>(quoll::Uuid{}),
            script.meta().uuid);
  EXPECT_TRUE(node["script"]["variables"]);

  EXPECT_FALSE(node["script"]["variables"]["test_str_invalid"]);
  EXPECT_EQ(
      node["script"]["variables"]["test_str"]["type"].as<quoll::String>(""),
      "string");
  EXPECT_EQ(
      node["script"]["variables"]["test_str"]["value"].as<quoll::String>(""),
      "hello world");

  EXPECT_EQ(
      node["script"]["variables"]["test_prefab"]["type"].as<quoll::String>(""),
      "prefab");
  EXPECT_EQ(node["script"]["variables"]["test_prefab"]["value"].as<quoll::Uuid>(
                quoll::Uuid{}),
            prefab.meta().uuid);

  EXPECT_EQ(
      node["script"]["variables"]["test_texture"]["type"].as<quoll::String>(""),
      "texture");
  EXPECT_EQ(
      node["script"]["variables"]["test_texture"]["value"].as<quoll::Uuid>(
          quoll::Uuid{}),
      texture.meta().uuid);
}

// Text
TEST_F(EntitySerializerTest,
       DoesNotCreateTextFieldIfTextComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfTextContentsAreEmpty) {
  auto font = createAsset<quoll::FontAsset>();

  auto entity = entityDatabase.create();
  quoll::Text component{};
  component.content = "";
  component.font = font;

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfFontAssetIsNotInRegistry) {
  auto entity = entityDatabase.create();

  quoll::Text component{};
  component.content = "Hello world";
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest,
       CreatesTextFieldIfTextContentsAreNotEmptyAndFontAssetIsInRegistry) {
  auto font = createAsset<quoll::FontAsset>();

  auto entity = entityDatabase.create();
  quoll::Text component{};
  component.content = "Hello world";
  component.lineHeight = 2.0f;
  component.font = font;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["text"]);
  EXPECT_TRUE(node["text"].IsMap());
  EXPECT_EQ(node["text"]["content"].as<quoll::String>(""), "Hello world");
  EXPECT_EQ(node["text"]["lineHeight"].as<f32>(-1.0f), component.lineHeight);
  EXPECT_EQ(node["text"]["font"].as<quoll::Uuid>(quoll::Uuid{}),
            font.meta().uuid);
}

// Rigid body
TEST_F(EntitySerializerTest,
       DoesNotCreateRigidBodyFieldIfRigidBodyComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["rigidBody"]);
}

TEST_F(EntitySerializerTest,
       CreatesDynamicRigidBodyFieldIfRigidBodyComponentWithDynamicTypeExists) {
  auto entity = entityDatabase.create();

  quoll::PhysicsDynamicRigidBodyDesc rigidBodyDesc{};
  rigidBodyDesc.applyGravity = true;
  rigidBodyDesc.inertia = glm::vec3(2.5f, 2.5f, 2.5f);
  rigidBodyDesc.mass = 4.5f;

  entityDatabase.set<quoll::RigidBody>(
      entity, {quoll::RigidBodyType::Dynamic, rigidBodyDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["rigidBody"]);
  EXPECT_EQ(node["rigidBody"]["type"].as<quoll::String>(""), "dynamic");
  EXPECT_EQ(node["rigidBody"]["applyGravity"].as<bool>(),
            rigidBodyDesc.applyGravity);
  EXPECT_EQ(node["rigidBody"]["inertia"].as<glm::vec3>(),
            rigidBodyDesc.inertia);
  EXPECT_EQ(node["rigidBody"]["mass"].as<f32>(), rigidBodyDesc.mass);
}

TEST_F(
    EntitySerializerTest,
    CreatesKinematicRigidBodyFieldIfRigidBodyComponentWithKinematicTypeExists) {
  auto entity = entityDatabase.create();

  quoll::PhysicsDynamicRigidBodyDesc rigidBodyDesc{};
  rigidBodyDesc.applyGravity = true;
  rigidBodyDesc.inertia = glm::vec3(2.5f, 2.5f, 2.5f);
  rigidBodyDesc.mass = 4.5f;

  entityDatabase.set<quoll::RigidBody>(
      entity, {quoll::RigidBodyType::Kinematic, rigidBodyDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["rigidBody"]);
  EXPECT_EQ(node["rigidBody"]["type"].as<quoll::String>(""), "kinematic");
  EXPECT_FALSE(node["rigidBody"]["applyGravity"]);
  EXPECT_FALSE(node["rigidBody"]["inertia"]);
  EXPECT_FALSE(node["rigidBody"]["mass"]);
}

// Collidable
TEST_F(EntitySerializerTest,
       DoesNotCreateCollidableFieldIfCollidableComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["collidable"]);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForBoxGeometry) {
  auto entity = entityDatabase.create();

  quoll::PhysicsGeometryBox boxGeometry{{2.5f, 3.5f, 4.5f}};

  quoll::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = quoll::PhysicsGeometryType::Box;
  geometryDesc.params = boxGeometry;
  geometryDesc.center = glm::vec3{2.5f, 3.5f, 4.5f};

  entityDatabase.set<quoll::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<quoll::String>(""), "box");
  EXPECT_EQ(node["collidable"]["center"].as<glm::vec3>(glm::vec3(0.0f)),
            geometryDesc.center);
  EXPECT_EQ(node["collidable"]["halfExtents"].as<glm::vec3>(glm::vec3(0.0f)),
            boxGeometry.halfExtents);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForSphereGeometry) {
  auto entity = entityDatabase.create();

  quoll::PhysicsGeometrySphere sphereParams{3.5f};

  quoll::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = quoll::PhysicsGeometryType::Sphere;
  geometryDesc.params = sphereParams;
  geometryDesc.center = glm::vec3{2.5f, 3.5f, 4.5f};

  entityDatabase.set<quoll::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<quoll::String>(""), "sphere");
  EXPECT_EQ(node["collidable"]["center"].as<glm::vec3>(glm::vec3(0.0f)),
            geometryDesc.center);
  EXPECT_EQ(node["collidable"]["radius"].as<f32>(0.0f), sphereParams.radius);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForCapsuleGeometry) {
  auto entity = entityDatabase.create();

  quoll::PhysicsGeometryCapsule capsuleParams{2.5f, 4.5f};

  quoll::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = quoll::PhysicsGeometryType::Capsule;
  geometryDesc.params = capsuleParams;
  geometryDesc.center = glm::vec3{2.5f, 3.5f, 4.5f};

  entityDatabase.set<quoll::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<quoll::String>(""), "capsule");
  EXPECT_EQ(node["collidable"]["center"].as<glm::vec3>(glm::vec3(0.0f)),
            geometryDesc.center);
  EXPECT_EQ(node["collidable"]["radius"].as<f32>(0.0f), capsuleParams.radius);
  EXPECT_EQ(node["collidable"]["halfHeight"].as<f32>(0.0f),
            capsuleParams.halfHeight);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForPlaneGeometry) {
  auto entity = entityDatabase.create();

  quoll::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = quoll::PhysicsGeometryType::Plane;
  geometryDesc.center = glm::vec3{2.5f, 3.5f, 4.5f};

  entityDatabase.set<quoll::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["center"].as<glm::vec3>(glm::vec3(0.0f)),
            geometryDesc.center);
  EXPECT_EQ(node["collidable"]["shape"].as<quoll::String>(""), "plane");
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldMaterialData) {
  auto entity = entityDatabase.create();

  quoll::PhysicsMaterialDesc materialDesc{};
  materialDesc.dynamicFriction = 2.5f;
  materialDesc.restitution = 4.5f;
  materialDesc.staticFriction = 3.5f;

  entityDatabase.set<quoll::Collidable>(entity, {{}, materialDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["dynamicFriction"].as<f32>(0.0f),
            materialDesc.dynamicFriction);
  EXPECT_EQ(node["collidable"]["restitution"].as<f32>(0.0f),
            materialDesc.restitution);
  EXPECT_EQ(node["collidable"]["staticFriction"].as<f32>(0.0f),
            materialDesc.staticFriction);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableWithDifferentSimulationMode) {
  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Collidable>(entity, {{}, {}, false});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(true), false);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(false), true);
}

TEST_F(EntitySerializerTest, CreatesCollidableWithDifferentQueryMode) {
  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Collidable>(entity, {{}, {}, true, false});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["useInSimulation"].as<bool>(false), true);
  EXPECT_EQ(node["collidable"]["useInQueries"].as<bool>(true), false);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentFieldIfParentComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentComponentIfParentEntityDoesNotExist) {
  static constexpr quoll::Entity NonExistentEntity{50};

  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Parent>(entity, {NonExistentEntity});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentComponentIfParentEntityDoesNotHaveAnId) {
  auto parent = entityDatabase.create();
  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Parent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest, CreatesEntityComponentIfParentIdExists) {
  static constexpr u64 ParentId{50};

  auto parent = entityDatabase.create();
  entityDatabase.set<quoll::Id>(parent, {ParentId});
  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Parent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_EQ(node["transform"]["parent"].as<u64>(0), ParentId);
}

// Skybox
TEST_F(EntitySerializerTest,
       DoesNotCreateSkyboxFieldIfEnvironmentSkyboxDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skybox"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateEnvironmentIfSkyboxIsTextureButAssetDoesNotExist) {
  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Texture};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skybox"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkyboxWithTextureColorIfTypeIsTextureAndAssetExists) {
  auto environment = createAsset<quoll::EnvironmentAsset>();

  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Texture,
                                     environment};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skybox"]);
  EXPECT_EQ(node["skybox"]["type"].as<quoll::String>(""), "texture");
  EXPECT_EQ(node["skybox"]["texture"].as<quoll::Uuid>(quoll::Uuid{}),
            environment.meta().uuid);
  EXPECT_FALSE(node["skybox"]["color"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkyboxWithColorTypeIfTypeIsColorAndAssetExists) {
  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Color,
                                     quoll::AssetRef<quoll::EnvironmentAsset>(),
                                     glm::vec4(0.2f, 0.3f, 0.4f, 0.5f)};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skybox"]);
  EXPECT_EQ(node["skybox"]["type"].as<quoll::String>(""), "color");
  EXPECT_EQ(node["skybox"]["color"].as<glm::vec4>(),
            glm::vec4(0.2f, 0.3f, 0.4f, 0.5f));
  EXPECT_FALSE(node["skybox"]["texture"]);
}

// Environment lighting
TEST_F(EntitySerializerTest,
       DoesNotCreateEnvironmentLightingFieldIfNoEnvironmentLightingComponent) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["environmentLighting"]);
}

TEST_F(EntitySerializerTest,
       CreateEnvironmentLightingWithSkyboxSourceIfComponentExists) {
  auto entity = entityDatabase.create();

  quoll::EnvironmentLightingSkyboxSource component{};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["environmentLighting"]);
  EXPECT_EQ(node["environmentLighting"]["source"].as<quoll::String>(""),
            "skybox");
}

TEST_F(EntitySerializerTest,
       DoesNotCreateInputMapFieldIfNoInputMapRefComponent) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["inputMap"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateInputMapFieldIfInputMapAssetDoesNotExist) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::InputMapAssetRef>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["inputMap"]);
}

TEST_F(EntitySerializerTest,
       CreatesInputMapFieldIfComponentExistsAndAssetIsValid) {
  auto inputMap = createAsset<quoll::InputMapAsset>();

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::InputMapAssetRef>(entity, {inputMap, 0});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["inputMap"]);
  EXPECT_EQ(node["inputMap"]["asset"].as<quoll::Uuid>(quoll::Uuid{}),
            inputMap.meta().uuid);
  EXPECT_EQ(node["inputMap"]["defaultScheme"].as<u32>(999), 0);
}

// UI Canvas
TEST_F(EntitySerializerTest, DoesNotCreateUiCanvasFieldIfNoUICanvasComponent) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["uiCanvas"]);
}

TEST_F(EntitySerializerTest, CreatesEmptyUICanvasMapIfUICanvasComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::UICanvas>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["uiCanvas"]);
  EXPECT_TRUE(node["uiCanvas"].IsMap());
}
