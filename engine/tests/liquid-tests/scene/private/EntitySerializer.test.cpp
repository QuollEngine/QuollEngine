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
  texture.relativePath = "/textures/sprite.ktx2";
  texture.name = "sprite.ktx2";
  auto handle = assetRegistry.getTextures().addAsset(texture);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Sprite>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["sprite"]);
  EXPECT_EQ(node["sprite"].as<liquid::String>(""),
            texture.relativePath.string());
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
  mesh.relativePath = "/meshes/mesh.lqmesh";
  mesh.name = "mesh.lqmesh";
  auto handle = assetRegistry.getMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Mesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["mesh"]);
  EXPECT_EQ(node["mesh"].as<liquid::String>(""), mesh.relativePath.string());
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
  mesh.relativePath = "/meshes/skinnedMesh.lqmesh";
  mesh.name = "skinnedMesh.lqmesh";
  auto handle = assetRegistry.getSkinnedMeshes().addAsset(mesh);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::SkinnedMesh>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skinnedMesh"]);
  EXPECT_EQ(node["skinnedMesh"].as<liquid::String>(""),
            mesh.relativePath.string());
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
  skeleton.relativePath = "/skeletons/skeleton.lqskel";
  skeleton.name = "skeleton.lqskel";
  auto handle = assetRegistry.getSkeletons().addAsset(skeleton);

  auto entity = entityDatabase.create();
  liquid::Skeleton component{};
  component.assetHandle = handle;

  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["skeleton"]);
  EXPECT_EQ(node["skeleton"].as<liquid::String>(),
            skeleton.relativePath.string());
}

// Animation
TEST_F(EntitySerializerTest,
       DoesNotCreateAnimatorFieldIfAnimatorComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_FALSE(node["skeleton"]);
}

TEST_F(EntitySerializerTest, CreatesAnimatorWithValidAnimations) {
  static constexpr uint32_t NonExistentHandleStart{45};
  static constexpr uint32_t NonExistentHandleEnd{55};

  liquid::Animator component{};

  component.currentAnimation = 2;
  for (uint32_t i = NonExistentHandleStart; i < NonExistentHandleEnd; ++i) {
    component.animations.push_back(
        static_cast<liquid::AnimationAssetHandle>(i));
  }

  std::vector<liquid::AnimationAssetHandle> validHandles;
  for (uint32_t i = 0; i < 5; ++i) {
    liquid::AssetData<liquid::AnimationAsset> animation{};
    animation.relativePath = "/animations/animation.lqanim";
    animation.name = "animation.lqanim";
    auto handle = assetRegistry.getAnimations().addAsset(animation);
    validHandles.push_back(handle);
    component.animations.push_back(handle);
  }

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, component);

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["animator"]);
  EXPECT_EQ(node["animator"]["startingAnimation"].as<uint32_t>(),
            component.currentAnimation);
  EXPECT_TRUE(node["animator"]["animations"].IsSequence());
  EXPECT_EQ(node["animator"]["animations"].size(), validHandles.size());

  for (size_t i = 0; i < node["animator"]["animations"].size(); ++i) {
    auto animation = node["animator"]["animations"][i];
    auto handle = validHandles.at(i);

    auto relPath = assetRegistry.getAnimations().getAsset(handle).relativePath;
    EXPECT_EQ(animation.as<liquid::String>(), relPath.string());
  }
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
  lens.fovY = 80.0f;
  entityDatabase.set(entity, lens);

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
  audio.relativePath = "/audios/bark.wav";
  audio.name = "bark.wav";
  auto handle = assetRegistry.getAudios().addAsset(audio);

  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::AudioSource>(entity, {handle});

  auto node = entitySerializer.createComponentsNode(entity);
  EXPECT_TRUE(node["audio"]);
  EXPECT_TRUE(node["audio"].IsMap());
  EXPECT_EQ(node["audio"]["source"].as<liquid::String>(""),
            audio.relativePath.string());
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
  script.relativePath = "/scripts/script.lua";
  script.name = "script.lua";
  script.data.variables.insert_or_assign(
      "test_str",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::String});
  script.data.variables.insert_or_assign(
      "test_prefab",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::AssetPrefab});
  auto handle = assetRegistry.getLuaScripts().addAsset(script);

  liquid::AssetData<liquid::PrefabAsset> prefab{};
  prefab.relativePath = "/prefabs/test.lqprefab";
  prefab.name = "test.lqprefab";
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
  EXPECT_EQ(node["script"]["asset"].as<liquid::String>(""),
            script.relativePath.string());
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
      prefab.relativePath.string());
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
  font.relativePath = "/fonts/Roboto.ttf";
  font.name = "Roboto.ttf";
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
  font.relativePath = "/fonts/Roboto.ttf";
  font.name = "Roboto.ttf";
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
  EXPECT_EQ(node["text"]["font"].as<liquid::String>(""),
            font.relativePath.string());
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
