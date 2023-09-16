#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/private/EntitySerializer.h"

class EntitySerializerTest : public ::testing::Test {
public:
  EntitySerializerTest() : entitySerializer(assetRegistry, entityDatabase) {}

public:
  quoll::AssetRegistry assetRegistry;
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
  static constexpr quoll::TextureAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Sprite>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesSpriteFieldIfTextureAssetIsInRegistry) {
  quoll::AssetData<quoll::TextureAsset> texture{};
  texture.uuid = quoll::Uuid("texture.tex");
  auto handle = assetRegistry.getTextures().addAsset(texture);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Sprite>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["sprite"]);
  EXPECT_EQ(node["sprite"].as<quoll::String>(""), "texture.tex");
}

// Mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshFieldIfMeshComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateMeshFieldIfMeshAssetIsNotInRegistry) {
  static constexpr quoll::MeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Mesh>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesMeshFieldIfMeshAssetIsInRegistry) {
  quoll::AssetData<quoll::MeshAsset> mesh{};
  mesh.uuid = quoll::Uuid("mesh.asset");

  auto handle = assetRegistry.getMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Mesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<quoll::String>(""), "mesh.asset");
}

// Mesh renderer
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshRendererFieldIfMeshRendererComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["meshRenderer"]);
}

TEST_F(EntitySerializerTest, CreatesMeshRendererFieldWithMaterials) {
  quoll::AssetData<quoll::MaterialAsset> material1{};
  material1.uuid = quoll::Uuid("material1.asset");

  quoll::AssetData<quoll::MaterialAsset> material2{};
  material2.uuid = quoll::Uuid("material2.asset");

  auto handle1 = assetRegistry.getMaterials().addAsset(material1);
  auto handle2 = assetRegistry.getMaterials().addAsset(material2);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(entity, {{handle1, handle2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<quoll::String>(""),
            "material1.asset");
  EXPECT_EQ(node["meshRenderer"]["materials"][1].as<quoll::String>(""),
            "material2.asset");
}

TEST_F(EntitySerializerTest,
       CreatesMeshRendererAndIgnoresNonExistentMaterials) {
  quoll::AssetData<quoll::MaterialAsset> material1{};
  material1.uuid = quoll::Uuid("material1.asset");
  auto handle1 = assetRegistry.getMaterials().addAsset(material1);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(
      entity, {{handle1, quoll::MaterialAssetHandle{25}}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<quoll::String>(""),
            "material1.asset");
}

TEST_F(EntitySerializerTest, CreatesMeshRendererWithNoMaterials) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::MeshRenderer>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 0);
}

// Skinned mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateSkinnedMeshFieldIfSkinnedMeshComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMesh"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateSkinnedMeshFieldIfSkinnedMeshAssetIsNotInRegistry) {
  static constexpr quoll::MeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMesh>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMesh"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshFieldIfSkinnedMeshAssetIsRegistry) {
  quoll::AssetData<quoll::MeshAsset> mesh{};
  mesh.uuid = quoll::Uuid("skinnedMesh.mesh");
  auto handle = assetRegistry.getMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<quoll::String>(""), "skinnedMesh.mesh");
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
  quoll::AssetData<quoll::MaterialAsset> material1{};
  material1.uuid = quoll::Uuid("material1.asset");

  quoll::AssetData<quoll::MaterialAsset> material2{};
  material2.uuid = quoll::Uuid("material2.asset");

  auto handle1 = assetRegistry.getMaterials().addAsset(material1);
  auto handle2 = assetRegistry.getMaterials().addAsset(material2);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMeshRenderer>(entity, {{handle1, handle2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<quoll::String>(""),
            "material1.asset");
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][1].as<quoll::String>(""),
            "material2.asset");
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshRendererAndIgnoresNonExistentMaterials) {
  quoll::AssetData<quoll::MaterialAsset> material1{};
  material1.uuid = quoll::Uuid("material1.asset");
  auto handle1 = assetRegistry.getMaterials().addAsset(material1);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::SkinnedMeshRenderer>(
      entity, {{handle1, quoll::MaterialAssetHandle{25}}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<quoll::String>(""),
            "material1.asset");
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
  static constexpr quoll::SkeletonAssetHandle NonExistentSkeletonHandle{45};

  auto entity = entityDatabase.create();
  quoll::Skeleton component{};
  component.assetHandle = NonExistentSkeletonHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest, CreatesSkeletonFieldIfSkeletonAssetIsInRegistry) {
  quoll::AssetData<quoll::SkeletonAsset> skeleton{};
  skeleton.uuid = quoll::Uuid("skeleton.skel");
  auto handle = assetRegistry.getSkeletons().addAsset(skeleton);

  auto entity = entityDatabase.create();
  quoll::Skeleton component{};
  component.assetHandle = handle;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skeleton"]);
  EXPECT_EQ(node["skeleton"].as<quoll::String>(), "skeleton.skel");
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
  EXPECT_EQ(node["jointAttachment"]["joint"].as<uint32_t>(0), 10);
}

// Animator
TEST_F(EntitySerializerTest,
       DoesNotCreateAnimatorFieldIfAnimatorAssetIsNotInRegistry) {
  static constexpr quoll::AnimatorAssetHandle NonExistentAnimatorHandle{45};

  auto entity = entityDatabase.create();
  quoll::Animator component{};
  component.asset = NonExistentAnimatorHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["animator"]);
}

TEST_F(EntitySerializerTest, CreatesAnimatorWithValidAnimations) {
  quoll::AssetData<quoll::AnimatorAsset> animator{};
  animator.uuid = quoll::Uuid("test.animator");
  auto handle = assetRegistry.getAnimators().addAsset(animator);

  quoll::Animator component{};
  component.asset = handle;
  component.currentState = 0;

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["animator"]);
  EXPECT_EQ(node["animator"]["asset"].as<quoll::String>(), "test.animator");
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
  EXPECT_EQ(node["light"]["type"].as<uint32_t>(1000), 0);
  EXPECT_EQ(node["light"]["intensity"].as<float>(-1.0f), light.intensity);
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
  EXPECT_EQ(node["light"]["shadow"]["splitLambda"].as<float>(1.0f),
            shadow.splitLambda);
  EXPECT_EQ(node["light"]["shadow"]["numCascades"].as<uint32_t>(0),
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
  EXPECT_EQ(node["light"]["type"].as<uint32_t>(1000), 1);
  EXPECT_EQ(node["light"]["intensity"].as<float>(-1.0f), light.intensity);
  EXPECT_EQ(node["light"]["color"].as<glm::vec4>(glm::vec4{-1.0f}),
            light.color);
  EXPECT_EQ(node["light"]["range"].as<float>(-1.0f), light.range);
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
  EXPECT_EQ(node["camera"]["type"].as<uint32_t>(1000), 0);
  EXPECT_EQ(node["camera"]["near"].as<float>(-1.0f), lens.near);
  EXPECT_EQ(node["camera"]["far"].as<float>(-1.0f), lens.far);
  EXPECT_EQ(node["camera"]["aspectRatio"].as<float>(-1.0f), lens.aspectRatio);
  EXPECT_EQ(node["camera"]["sensorSize"].as<glm::vec2>(glm::vec2{-1.0f, -1.0f}),
            lens.sensorSize);
  EXPECT_EQ(node["camera"]["focalLength"].as<float>(-1.0f), lens.focalLength);
  EXPECT_EQ(node["camera"]["aperture"].as<float>(-1.0f), lens.aperture);
  EXPECT_EQ(node["camera"]["shutterSpeed"].as<float>(-1.0f), lens.shutterSpeed);
  EXPECT_EQ(node["camera"]["sensitivity"].as<float>(-1.0f), lens.sensitivity);
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
  static constexpr quoll::AudioAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest, CreatesAudioFieldIfAudioAssetIsInRegistry) {
  quoll::AssetData<quoll::AudioAsset> audio{};
  audio.uuid = quoll::Uuid("bark.wav");
  auto handle = assetRegistry.getAudios().addAsset(audio);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AudioSource>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["audio"]);
  EXPECT_TRUE(node["audio"].IsMap());
  EXPECT_EQ(node["audio"]["source"].as<quoll::String>(""), "bark.wav");
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
  static constexpr quoll::LuaScriptAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Script>(entity, {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest, CreatesScriptFieldIfScriptAssetIsRegistry) {
  quoll::AssetData<quoll::LuaScriptAsset> script{};
  script.uuid = quoll::Uuid("script.lua");
  script.data.variables.insert_or_assign(
      "test_str",
      quoll::LuaScriptVariable{quoll::LuaScriptVariableType::String});
  script.data.variables.insert_or_assign(
      "test_prefab",
      quoll::LuaScriptVariable{quoll::LuaScriptVariableType::AssetPrefab});
  auto handle = assetRegistry.getLuaScripts().addAsset(script);

  quoll::AssetData<quoll::PrefabAsset> prefab{};
  prefab.uuid = quoll::Uuid("test.prefab");
  auto prefabHandle = assetRegistry.getPrefabs().addAsset(prefab);

  auto entity = entityDatabase.create();

  quoll::Script component{handle};
  component.variables.insert_or_assign("test_str",
                                       quoll::String("hello world"));
  component.variables.insert_or_assign("test_str_invalid",
                                       quoll::String("hello world"));
  component.variables.insert_or_assign("test_prefab", prefabHandle);
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["script"]);
  EXPECT_EQ(node["script"]["asset"].as<quoll::String>(""), "script.lua");
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

  EXPECT_EQ(
      node["script"]["variables"]["test_prefab"]["value"].as<quoll::String>(""),
      "test.prefab");
}

// Text
TEST_F(EntitySerializerTest,
       DoesNotCreateTextFieldIfTextComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfTextContentsAreEmpty) {
  quoll::AssetData<quoll::FontAsset> font{};
  font.uuid = quoll::Uuid("Roboto.ttf");
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.create();
  quoll::Text component{};
  component.text = "";
  component.font = handle;

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfFontAssetIsNotInRegistry) {
  static constexpr quoll::FontAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();

  quoll::Text component{};
  component.text = "Hello world";
  component.font = NonExistentHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest,
       CreatesTextFieldIfTextContentsAreNotEmptyAndFontAssetIsInRegistry) {
  quoll::AssetData<quoll::FontAsset> font{};
  font.uuid = quoll::Uuid("Roboto.ttf");
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.create();
  quoll::Text component{};
  component.text = "Hello world";
  component.lineHeight = 2.0f;
  component.font = handle;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["text"]);
  EXPECT_TRUE(node["text"].IsMap());
  EXPECT_EQ(node["text"]["content"].as<quoll::String>(""), "Hello world");
  EXPECT_EQ(node["text"]["lineHeight"].as<float>(-1.0f), component.lineHeight);
  EXPECT_EQ(node["text"]["font"].as<quoll::String>(""), "Roboto.ttf");
}

// Rigid body
TEST_F(EntitySerializerTest,
       DoesNotCreateRigidBodyFieldIfRigidBodyComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["rigidBody"]);
}

TEST_F(EntitySerializerTest, CreatesRigidBodyFieldIfRigidBodyComponentExists) {
  auto entity = entityDatabase.create();

  quoll::PhysicsDynamicRigidBodyDesc rigidBodyDesc{};
  rigidBodyDesc.applyGravity = true;
  rigidBodyDesc.inertia = glm::vec3(2.5f, 2.5f, 2.5f);
  rigidBodyDesc.mass = 4.5f;

  entityDatabase.set<quoll::RigidBody>(entity, {rigidBodyDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["rigidBody"]);
  EXPECT_EQ(node["rigidBody"]["applyGravity"].as<bool>(),
            rigidBodyDesc.applyGravity);
  EXPECT_EQ(node["rigidBody"]["inertia"].as<glm::vec3>(),
            rigidBodyDesc.inertia);
  EXPECT_EQ(node["rigidBody"]["mass"].as<float>(), rigidBodyDesc.mass);
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
  EXPECT_EQ(node["collidable"]["radius"].as<float>(0.0f), sphereParams.radius);
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
  EXPECT_EQ(node["collidable"]["radius"].as<float>(0.0f), capsuleParams.radius);
  EXPECT_EQ(node["collidable"]["halfHeight"].as<float>(0.0f),
            capsuleParams.halfHeight);
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
  EXPECT_EQ(node["collidable"]["dynamicFriction"].as<float>(0.0f),
            materialDesc.dynamicFriction);
  EXPECT_EQ(node["collidable"]["restitution"].as<float>(0.0f),
            materialDesc.restitution);
  EXPECT_EQ(node["collidable"]["staticFriction"].as<float>(0.0f),
            materialDesc.staticFriction);
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
  static constexpr uint64_t ParentId{50};

  auto parent = entityDatabase.create();
  entityDatabase.set<quoll::Id>(parent, {ParentId});
  auto entity = entityDatabase.create();

  entityDatabase.set<quoll::Parent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_EQ(node["transform"]["parent"].as<uint64_t>(0), ParentId);
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
  static constexpr quoll::EnvironmentAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Texture,
                                     NonExistentHandle};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skybox"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkyboxWithTextureColorIfTypeIsTextureAndAssetExists) {
  quoll::AssetData<quoll::EnvironmentAsset> data{};
  data.uuid = quoll::Uuid("uuid.env");
  auto handle = assetRegistry.getEnvironments().addAsset(data);

  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Texture,
                                     handle};
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skybox"]);
  EXPECT_EQ(node["skybox"]["type"].as<quoll::String>(""), "texture");
  EXPECT_EQ(node["skybox"]["texture"].as<quoll::String>(""), "uuid.env");
  EXPECT_FALSE(node["skybox"]["color"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkyboxWithColorTypeIfTypeIsColorAndAssetExists) {
  auto entity = entityDatabase.create();

  quoll::EnvironmentSkybox component{quoll::EnvironmentSkyboxType::Color,
                                     quoll::EnvironmentAssetHandle::Null,
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
