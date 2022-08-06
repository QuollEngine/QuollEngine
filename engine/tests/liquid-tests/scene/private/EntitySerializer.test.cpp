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
    auto entity = entityDatabase.createEntity();
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  }

  {
    auto entity = entityDatabase.createEntity();
    entityDatabase.setComponent<liquid::NameComponent>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled");
    EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  }
}

TEST_F(EntitySerializerTest,
       SetsNameToDefaultNameWithIdComponentIfNameIsEmpty) {
  {
    auto entity = entityDatabase.createEntity();
    entityDatabase.setComponent<liquid::IdComponent>(entity, {15});
    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  }

  {
    auto entity = entityDatabase.createEntity();
    entityDatabase.setComponent<liquid::IdComponent>(entity, {15});
    entityDatabase.setComponent<liquid::NameComponent>(entity, {""});

    auto node = entitySerializer.createComponentsNode(entity);

    EXPECT_TRUE(node["name"]);
    EXPECT_EQ(node["name"].as<liquid::String>(""), "Untitled 15");
    EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  }
}

TEST_F(EntitySerializerTest, CreatesNameFieldUsingNameComponentIfExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(entity, {"Test entity"});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["name"]);
  EXPECT_EQ(node["name"].as<liquid::String>(""), "Test entity");
}

// Transform
TEST_F(EntitySerializerTest,
       CreatesTransformFieldWithDefaultsIfTransformComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(
      entityDatabase.hasComponent<liquid::LocalTransformComponent>(entity));
  const auto &defaults =
      entityDatabase.getComponent<liquid::LocalTransformComponent>(entity);

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
       CreatesTransformFieldFromTransformComponetnIfExists) {
  auto entity = entityDatabase.createEntity();

  liquid::LocalTransformComponent transform{};
  transform.localPosition = glm::vec3{2.0f};
  transform.localRotation = glm::quat{0.5f, 0.5f, 0.5f, 0.5f};
  transform.localScale = glm::vec3{0.2f};

  entityDatabase.setComponent(entity, transform);

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

// Mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateMeshFieldIfMeshComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateMeshFieldIfMeshAssetIsNotInRegistry) {
  static constexpr liquid::MeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::MeshComponent>(entity,
                                                     {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["mesh"]);
}

TEST_F(EntitySerializerTest, CreatesMeshFieldIfMeshAssetIsInRegistry) {
  liquid::AssetData<liquid::MeshAsset> mesh{};
  mesh.relativePath = "/meshes/mesh.lqmesh";
  mesh.name = "mesh.lqmesh";
  auto handle = assetRegistry.getMeshes().addAsset(mesh);

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::MeshComponent>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<liquid::String>(""), mesh.relativePath.string());
}

// Skinned mesh
TEST_F(EntitySerializerTest,
       DoesNotCreateSkinnedMeshFieldIfSkinnedMeshComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMesh"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateSkinnedMeshFieldIfSkinnedMeshAssetIsNotInRegistry) {
  static constexpr liquid::SkinnedMeshAssetHandle NonExistentMeshHandle{45};

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::SkinnedMeshComponent>(
      entity, {NonExistentMeshHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skinnedMesh"]);
}

TEST_F(EntitySerializerTest,
       CreatesSkinnedMeshFieldIfSkinnedMeshAssetIsRegistry) {
  liquid::AssetData<liquid::SkinnedMeshAsset> mesh{};
  mesh.relativePath = "/meshes/skinnedMesh.lqmesh";
  mesh.name = "skinnedMesh.lqmesh";
  auto handle = assetRegistry.getSkinnedMeshes().addAsset(mesh);

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::SkinnedMeshComponent>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMesh"]);
  EXPECT_EQ(node["skinnedMesh"].as<liquid::String>(""),
            mesh.relativePath.string());
}

// Skeleton
TEST_F(EntitySerializerTest,
       DoesNotCreateSkeletonFieldIfSkeletonComponentDoesNotExist) {}

TEST_F(EntitySerializerTest,
       DoesNotCreateSkeletonFieldIfSkeletonAssetIsNotInRegistry) {
  static constexpr liquid::SkeletonAssetHandle NonExistentSkeletonHandle{45};

  auto entity = entityDatabase.createEntity();
  liquid::SkeletonComponent component{};
  component.assetHandle = NonExistentSkeletonHandle;
  entityDatabase.setComponent(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest, CreatesSkeletonFieldIfSkeletonAssetIsRegistry) {
  liquid::AssetData<liquid::SkeletonAsset> skeleton{};
  skeleton.relativePath = "/skeletons/skeleton.lqskel";
  skeleton.name = "skeleton.lqskel";
  auto handle = assetRegistry.getSkeletons().addAsset(skeleton);

  auto entity = entityDatabase.createEntity();
  liquid::SkeletonComponent component{};
  component.assetHandle = handle;

  entityDatabase.setComponent(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skeleton"]);
  EXPECT_EQ(node["skeleton"].as<liquid::String>(),
            skeleton.relativePath.string());
}

// Light
TEST_F(EntitySerializerTest, DoesNotCreateLightFieldIfNoLightComponent) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["light"]);
}

TEST_F(EntitySerializerTest,
       CreatesLightFieldIfDirectionalLightComponentExists) {
  auto entity = entityDatabase.createEntity();

  liquid::DirectionalLightComponent light{};
  light.intensity = 5.5f;
  light.color = glm::vec4{0.5f};
  entityDatabase.setComponent(entity, light);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["light"]);
  EXPECT_TRUE(node["light"].IsMap());
  EXPECT_EQ(node["light"]["type"].as<uint32_t>(1000), 0);
  EXPECT_EQ(node["light"]["intensity"].as<float>(-1.0f), light.intensity);
  EXPECT_EQ(node["light"]["color"].as<glm::vec4>(glm::vec4{-1.0f}),
            light.color);
  EXPECT_FALSE(node["light"]["direction"]);
}

// Camera
TEST_F(EntitySerializerTest,
       DoesNotCreateCameraFieldIfLensComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["camera"]);
}

TEST_F(EntitySerializerTest, CreatesCameraFieldIfLensComponentExists) {
  auto entity = entityDatabase.createEntity();

  liquid::PerspectiveLensComponent lens{};
  lens.aspectRatio = 2.5f;
  lens.far = 200.0f;
  lens.near = 0.2f;
  lens.fovY = 80.0f;
  entityDatabase.setComponent(entity, lens);

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["camera"]);
  EXPECT_TRUE(node["camera"].IsMap());
  EXPECT_EQ(node["camera"]["type"].as<uint32_t>(1000), 0);
  EXPECT_EQ(node["camera"]["fov"].as<float>(-1.0f), lens.fovY);
  EXPECT_EQ(node["camera"]["near"].as<float>(-1.0f), lens.near);
  EXPECT_EQ(node["camera"]["far"].as<float>(-1.0f), lens.far);
  EXPECT_EQ(node["camera"]["aspectRatio"].as<float>(-1.0f), lens.aspectRatio);
}

TEST_F(EntitySerializerTest,
       SetsCameraAspectRatioToAutoIfAutoAspectRatioComponentExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::PerspectiveLensComponent>(entity, {});
  entityDatabase.setComponent<liquid::AutoAspectRatioComponent>(entity, {});

  auto node = entitySerializer.createComponentsNode(entity);

  EXPECT_TRUE(node["camera"]);
  EXPECT_TRUE(node["camera"].IsMap());
  EXPECT_EQ(node["camera"]["aspectRatio"].as<liquid::String>(""), "auto");
}

// Script
TEST_F(EntitySerializerTest,
       DoesNotCreateAudioFieldIfAudioComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateAudioFieldIfAudioAssetIsNotInRegistry) {
  static constexpr liquid::AudioAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioSourceComponent>(
      entity, {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["audio"]);
}

TEST_F(EntitySerializerTest, CreatesAudioFieldIfAudioAssetIsInRegistry) {
  liquid::AssetData<liquid::AudioAsset> audio{};
  audio.relativePath = "/audios/bark.wav";
  audio.name = "bark.wav";
  auto handle = assetRegistry.getAudios().addAsset(audio);

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioSourceComponent>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["audio"]);
  EXPECT_TRUE(node["audio"].IsMap());
  EXPECT_EQ(node["audio"]["source"].as<liquid::String>(""),
            audio.relativePath.string());
}

// Script
TEST_F(EntitySerializerTest,
       DoesNotCreateScriptFieldIfScriptComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateScriptFieldIfScriptAssetIsNotInRegistry) {
  static constexpr liquid::LuaScriptAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity,
                                                          {NonExistentHandle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["script"]);
}

TEST_F(EntitySerializerTest, CreatesScriptFieldIfScriptAssetIsRegistry) {
  liquid::AssetData<liquid::LuaScriptAsset> script{};
  script.relativePath = "/scripts/script.lua";
  script.name = "script.lua";
  auto handle = assetRegistry.getLuaScripts().addAsset(script);

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["script"]);
  EXPECT_EQ(node["script"].as<liquid::String>(""),
            script.relativePath.string());
}

// Text
TEST_F(EntitySerializerTest,
       DoesNotCreateTextFieldIfTextComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfTextContentsAreEmpty) {
  liquid::AssetData<liquid::FontAsset> font{};
  font.relativePath = "/fonts/Roboto.ttf";
  font.name = "Roboto.ttf";
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.createEntity();
  liquid::TextComponent component{};
  component.text = "";
  component.font = handle;

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest, DoesNotCreateTextFieldIfFontAssetIsNotInRegistry) {
  static constexpr liquid::FontAssetHandle NonExistentHandle{45};

  auto entity = entityDatabase.createEntity();

  liquid::TextComponent component{};
  component.text = "Hello world";
  component.font = NonExistentHandle;
  entityDatabase.setComponent(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["text"]);
}

TEST_F(EntitySerializerTest,
       CreatesTextFieldIfTextContentsAreNotEmptyAndFontAssetIsInRegistry) {
  liquid::AssetData<liquid::FontAsset> font{};
  font.relativePath = "/fonts/Roboto.ttf";
  font.name = "Roboto.ttf";
  auto handle = assetRegistry.getFonts().addAsset(font);

  auto entity = entityDatabase.createEntity();
  liquid::TextComponent component{};
  component.text = "Hello world";
  component.lineHeight = 2.0f;
  component.font = handle;

  entityDatabase.setComponent(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["text"]);
  EXPECT_TRUE(node["text"].IsMap());
  EXPECT_EQ(node["text"]["content"].as<liquid::String>(""), "Hello world");
  EXPECT_EQ(node["text"]["lineHeight"].as<float>(-1.0f), component.lineHeight);
  EXPECT_EQ(node["text"]["font"].as<liquid::String>(""),
            font.relativePath.string());
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentFieldIfParentComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentComponentIfParentEntityDoesNotExist) {
  static constexpr liquid::Entity NonExistentEntity{50};

  auto entity = entityDatabase.createEntity();

  entityDatabase.setComponent<liquid::ParentComponent>(entity,
                                                       {NonExistentEntity});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest,
       DoesNotCreateParentComponentIfParentEntityDoesNotHaveAnId) {
  auto parent = entityDatabase.createEntity();
  auto entity = entityDatabase.createEntity();

  entityDatabase.setComponent<liquid::ParentComponent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["transform"]["parent"]);
}

TEST_F(EntitySerializerTest, CreatesEntityComponentIfParentIdExists) {
  static constexpr uint64_t ParentId{50};

  auto parent = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::IdComponent>(parent, {ParentId});
  auto entity = entityDatabase.createEntity();

  entityDatabase.setComponent<liquid::ParentComponent>(entity, {parent});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_EQ(node["transform"]["parent"].as<uint64_t>(0), ParentId);
}
