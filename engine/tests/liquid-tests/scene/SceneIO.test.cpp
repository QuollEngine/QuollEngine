#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/entity/EntityDatabase.h"
#include "liquid/scene/SceneIO.h"

class SceneIOTest : public ::testing::Test {
public:
  static const liquid::Path SceneDirectory;

public:
  SceneIOTest() : sceneIO(assetRegistry, entityDatabase) {}

  void SetUp() override { std::filesystem::create_directory(SceneDirectory); }

  void TearDown() override { std::filesystem::remove_all(SceneDirectory); }

  liquid::String getEntityFile(liquid::Entity entity) {
    auto id = entityDatabase.getComponent<liquid::IdComponent>(entity).id;
    return std::to_string(id) + ".lqnode";
  }

  void writeNodeToFile(const YAML::Node &node, const liquid::String &path) {
    std::ofstream stream(SceneDirectory / (path + ".lqnode"));
    stream << node;
    stream.close();
  }

  void createSimpleEntityFile(uint64_t id, uint64_t parent = 0) {
    YAML::Node node;
    node["id"] = id;
    node["version"] = "0.1";

    if (parent > 0) {
      node["components"]["transform"]["parent"] = parent;
    }

    writeNodeToFile(node, std::to_string(id));
  }

public:
  liquid::AssetRegistry assetRegistry;
  liquid::EntityDatabase entityDatabase;
  liquid::SceneIO sceneIO;
};

const liquid::Path SceneIOTest::SceneDirectory =
    std::filesystem::current_path() / "scene-io-test";

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfNodeDoesNotHaveId) {
  YAML::Node node;
  node["version"] = "0.1";

  writeNodeToFile(node, "test");
  sceneIO.loadScene(SceneDirectory);

  EXPECT_EQ(entityDatabase.getEntityCount(), 0);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsInvalid) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Undefined),
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Map),
      YAML::Node(YAML::NodeType::Sequence),
      YAML::Node(YAML::NodeType::Scalar),
  };

  for (const auto &invalidNode : invalidNodes) {
    YAML::Node node;
    node["version"] = "0.1";
    node["id"] = invalidNode;
    writeNodeToFile(node, "test");
    sceneIO.loadScene(SceneDirectory);

    EXPECT_EQ(entityDatabase.getEntityCount(), 0);
  }
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsZero) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 0;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(SceneDirectory);

  EXPECT_EQ(entityDatabase.getEntityCount(), 0);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdIsNegative) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = -1;
  writeNodeToFile(node, "test");
  sceneIO.loadScene(SceneDirectory);

  EXPECT_EQ(entityDatabase.getEntityCount(), 0);
}

TEST_F(SceneIOTest, DoesNotCreateEntityFromNodeIfIdAlreadyExists) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = 50;

  writeNodeToFile(node, "test");
  sceneIO.loadScene(SceneDirectory);

  // first one is added
  EXPECT_EQ(entityDatabase.getEntityCount(), 1);

  sceneIO.loadScene(SceneDirectory);

  // second addition will fail due to duplicate Id
  EXPECT_EQ(entityDatabase.getEntityCount(), 1);
}

TEST_F(SceneIOTest, LoadingSceneFilesFromDirectoryCreatesOneEntityPerFile) {
  constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    createSimpleEntityFile(i);
  }

  const auto &entities = sceneIO.loadScene(SceneDirectory);

  EXPECT_EQ(entityDatabase.getEntityCount(), entities.size());
  for (auto entity : entities) {
    EXPECT_TRUE(entityDatabase.hasComponent<liquid::IdComponent>(entity));
  }
}

TEST_F(SceneIOTest, SavingSceneAfterLoadingCreatesEntityWithNonConflictingId) {
  constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    createSimpleEntityFile(i);
  }

  sceneIO.loadScene(SceneDirectory);

  auto entity = entityDatabase.createEntity();

  sceneIO.saveEntity(entity, SceneDirectory);

  auto id = entityDatabase.getComponent<liquid::IdComponent>(entity).id;

  EXPECT_EQ(id, 10);
  EXPECT_TRUE(std::filesystem::is_regular_file(SceneDirectory / "10.lqnode"));
}

TEST_F(SceneIOTest, LoadingSetsParentsProperly) {
  constexpr uint64_t NumEntities = 9;

  for (uint64_t i = 1; i < NumEntities + 1; ++i) {
    // set parent to next entity
    // to make sure that parent entities
    // are loaded after child ones
    createSimpleEntityFile(i, i + 1);
  }

  const auto &entities = sceneIO.loadScene(SceneDirectory);

  EXPECT_EQ(entityDatabase.getEntityCount(), entities.size());
  EXPECT_EQ(
      entityDatabase.getEntityCountForComponent<liquid::ParentComponent>(),
      entities.size() - 1);
}

TEST_F(SceneIOTest, LoadingSetsParentsFromPreviousSaveProperly) {
  constexpr uint64_t NumEntities = 9;

  auto parent = entityDatabase.createEntity();

  // Creates ID for the parent and store it in cache
  sceneIO.saveEntity(parent, SceneDirectory);

  auto parentId = entityDatabase.getComponent<liquid::IdComponent>(parent).id;

  createSimpleEntityFile(10, parentId);

  sceneIO.loadScene(SceneDirectory);

  auto lastAddedEntity =
      static_cast<liquid::Entity>(entityDatabase.getEntityCount());

  EXPECT_TRUE(
      entityDatabase.hasComponent<liquid::ParentComponent>(lastAddedEntity));
  EXPECT_EQ(
      entityDatabase.getComponent<liquid::ParentComponent>(lastAddedEntity)
          .parent,
      parent);
}

TEST_F(SceneIOTest, SavingEntityCreatesIdComponentIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  sceneIO.saveEntity(entity, SceneDirectory);

  EXPECT_TRUE(entityDatabase.hasComponent<liquid::IdComponent>(entity));
}

TEST_F(SceneIOTest,
       SavingNewEntityCreatesEntityFileIfProvidedPathIsADirectory) {
  auto entity = entityDatabase.createEntity();

  sceneIO.saveEntity(entity, SceneDirectory);

  auto path = SceneDirectory / getEntityFile(entity);
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST_F(SceneIOTest,
       SavingExistingEntityCreatesEntityFileIfProvidedPathIsADirectory) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::IdComponent>(entity, {155});
  sceneIO.saveEntity(entity, SceneDirectory);

  auto path = SceneDirectory / "155.lqnode";
  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}
