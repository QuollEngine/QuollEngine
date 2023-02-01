#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquidator-tests/Testing.h"
#include "liquidator/asset/GLTFImporter.h"

#include "GLTFImporterTestBase.h"

class GLTFImporterSkeletonTest : public GLTFImporterTestBase {
public:
  void createNodes(GLTFTestScene &scene, size_t nodeIndex, int currentLevel,
                   int maxLevel) {
    if (currentLevel >= maxLevel)
      return;

    auto scale = glm::vec3{2.0f};
    auto position = glm::vec3(static_cast<float>(currentLevel));

    scene.nodes.push_back(GLTFTestNode{position, scale});
    scene.nodes.at(nodeIndex).children.push_back(
        static_cast<int>(scene.nodes.size() - 1));
    createNodes(scene, scene.nodes.size() - 1, currentLevel + 1, maxLevel);

    scene.nodes.push_back(GLTFTestNode{position, -scale});
    scene.nodes.at(nodeIndex).children.push_back(
        static_cast<int>(scene.nodes.size() - 1));

    createNodes(scene, scene.nodes.size() - 1, currentLevel + 1, maxLevel);
  }

  GLTFTestSkin &createSimpleSkin(GLTFTestScene &scene) {
    scene.nodes.push_back({glm::vec3{0.0f}, glm::vec3{1.0f}});
    size_t startIndex = scene.nodes.size() - 1;

    createNodes(scene, scene.nodes.size() - 1, 0, 2);

    GLTFTestSkin skin;
    skin.inverseBindMatrices.data.resize(7);
    for (size_t i = startIndex; i < startIndex + 7; ++i) {
      skin.joints.push_back(static_cast<int>(i));
    }
    scene.skins.push_back(skin);

    return scene.skins.at(scene.skins.size() - 1);
  }
};

TEST_F(GLTFImporterSkeletonTest, DoesNotCreateSkeletonIfNoSkins) {
  GLTFTestScene scene;

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getSkeletons().getAssets().empty());
}

TEST_F(GLTFImporterSkeletonTest,
       DoesNotCreateSkeletonIfInverseBindMatricesComponentTypeIsNotFloat) {
  std::array<int, 7> componentTypes{TINYGLTF_COMPONENT_TYPE_BYTE,
                                    TINYGLTF_COMPONENT_TYPE_SHORT,
                                    TINYGLTF_COMPONENT_TYPE_INT,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
                                    TINYGLTF_COMPONENT_TYPE_DOUBLE};
  GLTFTestScene scene;

  for (auto componentType : componentTypes) {
    auto &skin = createSimpleSkin(scene);
    skin.inverseBindMatrices.componentType = componentType;
  }

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getSkeletons().getAssets().empty());
}

TEST_F(GLTFImporterSkeletonTest,
       DoesNotCreateSkeletonIfInverseBindMatricesTypeIsNotMat4) {
  std::array<int, 6> types{TINYGLTF_TYPE_SCALAR, TINYGLTF_TYPE_VEC2,
                           TINYGLTF_TYPE_VEC3,   TINYGLTF_TYPE_VEC4,
                           TINYGLTF_TYPE_MAT2,   TINYGLTF_TYPE_MAT3};

  GLTFTestScene scene;

  for (auto type : types) {
    auto &skin = createSimpleSkin(scene);
    skin.inverseBindMatrices.type = type;
  }

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getSkeletons().getAssets().empty());
}

TEST_F(
    GLTFImporterSkeletonTest,
    DoesNotCreateSkeletonIfNumberOfInverseBindMatricesAreDifferentThanNumberOfJoints) {

  GLTFTestScene scene;

  {
    auto &skin = createSimpleSkin(scene);
    skin.inverseBindMatrices.data.pop_back();
  }

  {
    auto &skin = createSimpleSkin(scene);
    skin.inverseBindMatrices.data.push_back({});
  }

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getSkeletons().getAssets().empty());
}

TEST_F(GLTFImporterSkeletonTest,
       DoesNotCreateSkeletonIfJointBelongsToTwoSkins) {

  GLTFTestScene scene;

  int jointIndex = -1;
  {
    auto &skin = createSimpleSkin(scene);

    jointIndex = skin.joints.at(3);
  }

  {
    auto &skin = createSimpleSkin(scene);

    skin.joints.at(3) = jointIndex;
  }

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  // First skin is created while second one is ignored
  EXPECT_EQ(assetCache.getRegistry().getSkeletons().getAssets().size(), 1);
}

TEST_F(GLTFImporterSkeletonTest, CreatesSkeletonWithJoints) {
  GLTFTestScene scene;

  auto &skin = createSimpleSkin(scene);

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getSkeletons().getAssets().empty());

  auto &skeleton = assetCache.getRegistry().getSkeletons().getAsset(
      liquid::SkeletonAssetHandle{1});

  for (size_t i = 0; i < skeleton.data.jointLocalPositions.size(); ++i) {
    auto &node = scene.nodes.at(skin.joints.at(i));

    auto &position = skeleton.data.jointLocalPositions.at(i);
    auto &scale = skeleton.data.jointLocalScales.at(i);

    EXPECT_EQ(position, node.position);
    EXPECT_EQ(scale, node.scale);
  }
}