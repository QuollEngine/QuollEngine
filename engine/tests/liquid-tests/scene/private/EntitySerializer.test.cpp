#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/private/EntitySerializer.h"

class EntitySerializerTest : public ::testing::Test {
public:
  EntitySerializerTest() : entitySerializer(assetRegistry, entityDatabase) {}

public:
  liquid::AssetRegistry assetRegistry;
  liquid::EntityDatabase entityDatabase;
  liquid::detail::EntitySerializer entitySerializer;
};

// Name
TEST_F(EntitySerializerTest,
       SetsNameToDefaultNameWithoutIdComponentIfNameIsEmpty) {
  {
    auto entity = entityDatabase.create();
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.has<liquid::Name>(entity));
  }

  {
    auto entity = entityDatabase.create();
    entityDatabase.set<liquid::Name>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.has<liquid::Name>(entity));
  }
}

TEST_F(EntitySerializerTest,
       SetsNameToDefaultNameWithIdComponentIfNameIsEmpty) {
  {
    auto entity = entityDatabase.create();
    entityDatabase.set<liquid::Id>(entity, {15});
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.has<liquid::Name>(entity));
  }

  {
    auto entity = entityDatabase.create();
    entityDatabase.set<liquid::Id>(entity, {15});
    entityDatabase.set<liquid::Name>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.has<liquid::Name>(entity));
  }
}

TEST_F(EntitySerializerTest, CreatesNameFieldUsingNameComponentIfExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity, {"Test entity"});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["name"]);
  EXPECT_EQ(node["name"].as<liquid::String>(""), "Test entity");
}

// Transform
TEST_F(EntitySerializerTest,
       CreatesTransformFieldWithDefaultsIfTransformComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(entityDatabase.has<liquid::LocalTransform>(entity));
  const auto &defaults = entityDatabase.get<liquid::LocalTransform>(entity);

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

  liquid::LocalTransform transform{};
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
  static constexpr liquid::TextureAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Sprite>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesSpriteFieldIfTextureAssetIsInRegistry) {
  liquid::AssetData<liquid::TextureAsset> texture{};
  texture.uuid = "texture.tex";
  auto handle = assetRegistry.getTextures().addAsset(texture);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Sprite>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["sprite"]);
  EXPECT_EQ(node["sprite"].as<liquid::String>(""), "texture.tex");
}

// Mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshFieldIfMeshComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateMeshFieldIfMeshAssetIsNotInRegistry) {
  static constexpr liquid::MeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Mesh>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesMeshFieldIfMeshAssetIsInRegistry) {
  liquid::AssetData<liquid::MeshAsset> mesh{};
  mesh.uuid = "mesh.asset";

  auto handle = assetRegistry.getMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Mesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<liquid::String>(""), "mesh.asset");
}

// Mesh renderer
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshRendererFieldIfMeshRendererComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["meshRenderer"]);
}

TEST_F(EntitySerializerTest, CreatesMeshRendererFieldWithMaterials) {
  liquid::AssetData<liquid::MaterialAsset> material1{};
  material1.uuid = "material1.asset";

  liquid::AssetData<liquid::MaterialAsset> material2{};
  material2.uuid = "material2.asset";

  auto handle1 = assetRegistry.getMaterials().addAsset(material1);
  auto handle2 = assetRegistry.getMaterials().addAsset(material2);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::MeshRenderer>(entity, {{handle1, handle2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<liquid::String>(""),
            "material1.asset");
  EXPECT_EQ(node["meshRenderer"]["materials"][1].as<liquid::String>(""),
            "material2.asset");
}

TEST_F(EntitySerializerTest,
       CreatesMeshRendererAndIgnoresNonExistentMaterials) {
  liquid::AssetData<liquid::MaterialAsset> material1{};
  material1.uuid = "material1.asset";
  auto handle1 = assetRegistry.getMaterials().addAsset(material1);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::MeshRenderer>(
      entity, {{handle1, liquid::MaterialAssetHandle{25}}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["meshRenderer"]);
  EXPECT_TRUE(node["meshRenderer"]["materials"]);
  EXPECT_EQ(node["meshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["meshRenderer"]["materials"][0].as<liquid::String>(""),
            "material1.asset");
}

TEST_F(EntitySerializerTest, CreatesMeshRendererWithNoMaterials) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::MeshRenderer>(entity, {});

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
  static constexpr liquid::SkinnedMeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMesh>(entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMesh"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshFieldIfSkinnedMeshAssetIsRegistry) {
  liquid::AssetData<liquid::SkinnedMeshAsset> mesh{};
  mesh.uuid = "skinnedMesh.mesh";
  auto handle = assetRegistry.getSkinnedMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMesh"]);
  EXPECT_EQ(node["skinnedMesh"].as<liquid::String>(""), "skinnedMesh.mesh");
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
  liquid::AssetData<liquid::MaterialAsset> material1{};
  material1.uuid = "material1.asset";

  liquid::AssetData<liquid::MaterialAsset> material2{};
  material2.uuid = "material2.asset";

  auto handle1 = assetRegistry.getMaterials().addAsset(material1);
  auto handle2 = assetRegistry.getMaterials().addAsset(material2);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMeshRenderer>(entity, {{handle1, handle2}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 2);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<liquid::String>(""),
            "material1.asset");
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][1].as<liquid::String>(""),
            "material2.asset");
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshRendererAndIgnoresNonExistentMaterials) {
  liquid::AssetData<liquid::MaterialAsset> material1{};
  material1.uuid = "material1.asset";
  auto handle1 = assetRegistry.getMaterials().addAsset(material1);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMeshRenderer>(
      entity, {{handle1, liquid::MaterialAssetHandle{25}}});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMeshRenderer"]);
  EXPECT_TRUE(node["skinnedMeshRenderer"]["materials"]);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"].size(), 1);
  EXPECT_EQ(node["skinnedMeshRenderer"]["materials"][0].as<liquid::String>(""),
            "material1.asset");
}

TEST_F(EntitySerializerTest, CreatesSkinnedMeshRendererWithNoMaterials) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMeshRenderer>(entity, {});

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
  static constexpr liquid::SkeletonAssetHandle NonExistentSkeletonHandle{45};

  auto entity = entityDatabase.create();
  liquid::Skeleton component{};
  component.assetHandle = NonExistentSkeletonHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest, CreatesSkeletonFieldIfSkeletonAssetIsInRegistry) {
  liquid::AssetData<liquid::SkeletonAsset> skeleton{};
  skeleton.uuid = "skeleton.skel";
  auto handle = assetRegistry.getSkeletons().addAsset(skeleton);

  auto entity = entityDatabase.create();
  liquid::Skeleton component{};
  component.assetHandle = handle;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skeleton"]);
  EXPECT_EQ(node["skeleton"].as<liquid::String>(), "skeleton.skel");
}

// Animator
TEST_F(EntitySerializerTest,
       DoesNotCreateAnimatorFieldIfAnimatorComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["animator"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateAnimatorFieldIfAnimatorAssetIsNotInRegistry) {
  static constexpr liquid::AnimatorAssetHandle NonExistentAnimatorHandle{45};

  auto entity = entityDatabase.create();
  liquid::Animator component{};
  component.asset = NonExistentAnimatorHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["animator"]);
}

TEST_F(EntitySerializerTest, CreatesAnimatorWithValidAnimations) {
  liquid::AssetData<liquid::AnimatorAsset> animator{};
  animator.uuid = "test.animator";
  auto handle = assetRegistry.getAnimators().addAsset(animator);

  liquid::Animator component{};
  component.asset = handle;
  component.currentState = 0;

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["animator"]);
  EXPECT_EQ(node["animator"]["asset"].as<liquid::String>(), "test.animator");
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

  liquid::DirectionalLight light{};
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
  entityDatabase.set<liquid::CascadedShadowMap>(entity, {});
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["light"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateShadowFieldInLightIfNoCascadedShadowComponent) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::DirectionalLight>(entity, {});
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["light"]);
  EXPECT_FALSE(node["light"]["shadow"]);
}

TEST_F(
    EntitySerializerTest,
    CreatesShadowFieldInLightIfDirectionalLightComponentAndCascadedShadowMapComponentsExist) {
  auto entity = entityDatabase.create();

  liquid::DirectionalLight light{};
  entityDatabase.set(entity, light);

  liquid::CascadedShadowMap shadow{};
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

  liquid::PointLight light{};
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

  liquid::PerspectiveLens lens{};
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
  entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["camera"]);
  EXPECT_TRUE(node["camera"].IsMap());
  EXPECT_EQ(node["camera"]["aspectRatio"].as<liquid::String>(""), "auto");
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
  static constexpr liquid::AudioAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::AudioSource>(entity, {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest, CreatesAudioFieldIfAudioAssetIsInRegistry) {
  liquid::AssetData<liquid::AudioAsset> audio{};
  audio.uuid = "bark.wav";
  auto handle = assetRegistry.getAudios().addAsset(audio);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::AudioSource>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["audio"]);
  EXPECT_TRUE(node["audio"].IsMap());
  EXPECT_EQ(node["audio"]["source"].as<liquid::String>(""), "bark.wav");
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
  static constexpr liquid::LuaScriptAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Script>(entity, {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest, CreatesScriptFieldIfScriptAssetIsRegistry) {
  liquid::AssetData<liquid::LuaScriptAsset> script{};
  script.uuid = "script.lua";
  script.data.variables.insert_or_assign(
      "test_str",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::String});
  script.data.variables.insert_or_assign(
      "test_prefab",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::AssetPrefab});
  auto handle = assetRegistry.getLuaScripts().addAsset(script);

  liquid::AssetData<liquid::PrefabAsset> prefab{};
  prefab.uuid = "test.lqprefab";
  auto prefabHandle = assetRegistry.getPrefabs().addAsset(prefab);

  auto entity = entityDatabase.create();

  liquid::Script component{handle};
  component.variables.insert_or_assign("test_str",
                                       liquid::String("hello world"));
  component.variables.insert_or_assign("test_str_invalid",
                                       liquid::String("hello world"));
  component.variables.insert_or_assign("test_prefab", prefabHandle);
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["script"]);
  EXPECT_EQ(node["script"]["asset"].as<liquid::String>(""), "script.lua");
  EXPECT_TRUE(node["script"]["variables"]);

  EXPECT_FALSE(node["script"]["variables"]["test_str_invalid"]);
  EXPECT_EQ(
      node["script"]["variables"]["test_str"]["type"].as<liquid::String>(""),
      "string");
  EXPECT_EQ(
      node["script"]["variables"]["test_str"]["value"].as<liquid::String>(""),
      "hello world");
  EXPECT_EQ(
      node["script"]["variables"]["test_prefab"]["type"].as<liquid::String>(""),
      "prefab");

  EXPECT_EQ(
      node["script"]["variables"]["test_prefab"]["value"].as<liquid::String>(
          ""),
      "test.lqprefab");
}

// Text
TEST_F(EntitySerializerTest,
       DoesNotCreateTextFieldIfTextComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfTextContentsAreEmpty) {
  liquid::AssetData<liquid::FontAsset> font{};
  font.uuid = "Roboto.ttf";
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.create();
  liquid::Text component{};
  component.text = "";
  component.font = handle;

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfFontAssetIsNotInRegistry) {
  static constexpr liquid::FontAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.create();

  liquid::Text component{};
  component.text = "Hello world";
  component.font = NonExistentHandle;
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest,
       CreatesTextFieldIfTextContentsAreNotEmptyAndFontAssetIsInRegistry) {
  liquid::AssetData<liquid::FontAsset> font{};
  font.uuid = "Roboto.ttf";
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.create();
  liquid::Text component{};
  component.text = "Hello world";
  component.lineHeight = 2.0f;
  component.font = handle;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["text"]);
  EXPECT_TRUE(node["text"].IsMap());
  EXPECT_EQ(node["text"]["content"].as<liquid::String>(""), "Hello world");
  EXPECT_EQ(node["text"]["lineHeight"].as<float>(-1.0f), component.lineHeight);
  EXPECT_EQ(node["text"]["font"].as<liquid::String>(""), "Roboto.ttf");
}

TEST_F(EntitySerializerTest,
       DoesNotCreateRigidBodyFieldIfRigidBodyComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["rigidBody"]);
}

TEST_F(EntitySerializerTest, CreatesRigidBodyFieldIfRigidBodyComponentExists) {
  auto entity = entityDatabase.create();

  liquid::PhysicsDynamicRigidBodyDesc rigidBodyDesc{};
  rigidBodyDesc.applyGravity = true;
  rigidBodyDesc.inertia = glm::vec3(2.5f, 2.5f, 2.5f);
  rigidBodyDesc.mass = 4.5f;

  entityDatabase.set<liquid::RigidBody>(entity, {rigidBodyDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["rigidBody"]);
  EXPECT_EQ(node["rigidBody"]["applyGravity"].as<bool>(),
            rigidBodyDesc.applyGravity);
  EXPECT_EQ(node["rigidBody"]["inertia"].as<glm::vec3>(),
            rigidBodyDesc.inertia);
  EXPECT_EQ(node["rigidBody"]["mass"].as<float>(), rigidBodyDesc.mass);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateCollidableFieldIfCollidableComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["collidable"]);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForBoxGeometry) {
  auto entity = entityDatabase.create();

  liquid::PhysicsGeometryBox boxGeometry{{2.5f, 3.5f, 4.5f}};

  liquid::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = liquid::PhysicsGeometryType::Box;
  geometryDesc.params = boxGeometry;

  entityDatabase.set<liquid::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<liquid::String>(""), "box");
  EXPECT_EQ(node["collidable"]["halfExtents"].as<glm::vec3>(glm::vec3(0.0f)),
            boxGeometry.halfExtents);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForSphereGeometry) {
  auto entity = entityDatabase.create();

  liquid::PhysicsGeometrySphere sphereParams{3.5f};

  liquid::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = liquid::PhysicsGeometryType::Sphere;
  geometryDesc.params = sphereParams;

  entityDatabase.set<liquid::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<liquid::String>(""), "sphere");
  EXPECT_EQ(node["collidable"]["radius"].as<float>(0.0f), sphereParams.radius);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForCapsuleGeometry) {
  auto entity = entityDatabase.create();

  liquid::PhysicsGeometryCapsule capsuleParams{2.5f, 4.5f};

  liquid::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = liquid::PhysicsGeometryType::Capsule;
  geometryDesc.params = capsuleParams;

  entityDatabase.set<liquid::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<liquid::String>(""), "capsule");
  EXPECT_EQ(node["collidable"]["radius"].as<float>(0.0f), capsuleParams.radius);
  EXPECT_EQ(node["collidable"]["halfHeight"].as<float>(0.0f),
            capsuleParams.halfHeight);
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldForPlaneGeometry) {
  auto entity = entityDatabase.create();

  liquid::PhysicsGeometryDesc geometryDesc{};
  geometryDesc.type = liquid::PhysicsGeometryType::Plane;

  entityDatabase.set<liquid::Collidable>(entity, {geometryDesc});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["collidable"]);
  EXPECT_EQ(node["collidable"]["shape"].as<liquid::String>(""), "plane");
}

TEST_F(EntitySerializerTest, CreatesCollidableFieldMaterialData) {
  auto entity = entityDatabase.create();

  liquid::PhysicsMaterialDesc materialDesc{};
  materialDesc.dynamicFriction = 2.5f;
  materialDesc.restitution = 4.5f;
  materialDesc.staticFriction = 3.5f;

  entityDatabase.set<liquid::Collidable>(entity, {{}, materialDesc});

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
  static constexpr liquid::Entity NonExistentEntity{50};

  auto entity = entityDatabase.create();

  entityDatabase.set<liquid::Parent>(entity, {NonExistentEntity});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentComponentIfParentEntityDoesNotHaveAnId) {
  auto parent = entityDatabase.create();
  auto entity = entityDatabase.create();

  entityDatabase.set<liquid::Parent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest, CreatesEntityComponentIfParentIdExists) {
  static constexpr uint64_t ParentId{50};

  auto parent = entityDatabase.create();
  entityDatabase.set<liquid::Id>(parent, {ParentId});
  auto entity = entityDatabase.create();

  entityDatabase.set<liquid::Parent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_EQ(node["transform"]["parent"].as<uint64_t>(0), ParentId);
}
