#include "liquid/core/Base.h"
#include "liquid/scene/Scene.h"
#include "liquid/scene/Camera.h"
#include "liquid/scene/Light.h"

#include <gtest/gtest.h>

class SceneTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
  liquid::EntityContext context;
};

using SceneDeathTest = SceneTest;

TEST_F(SceneTest, CreatesEmptyRootNodeOnConstruct) {
  liquid::Scene scene(context);

  auto *rootNode = scene.getRootNode();

  scene.update();

  EXPECT_NE(rootNode->getEntity(), std::numeric_limits<liquid::Entity>::max());
  EXPECT_EQ(rootNode->getParent(), nullptr);
  EXPECT_TRUE(rootNode->getWorldTransform() == glm::mat4{1.0f});
}

TEST_F(SceneTest, SetsActiveCameraEntity) {
  liquid::Scene scene(context);
  auto entity = context.createEntity();
  context.setComponent<liquid::CameraComponent>(
      entity, {std::make_shared<liquid::Camera>(&registry)});
  scene.setActiveCamera(entity);
  EXPECT_EQ(scene.getActiveCamera().get(),
            context.getComponent<liquid::CameraComponent>(entity).camera.get());
}

TEST_F(SceneDeathTest,
       SetingActiveCameraEntityFailsIfEntityHasNoCameraComponent) {
  liquid::EntityContext context;
  liquid::Scene scene(context);
  EXPECT_DEATH(scene.setActiveCamera(context.createEntity()), ".*");
}
