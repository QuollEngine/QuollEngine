#include "liquid/core/Base.h"
#include "liquid/scene/Scene.h"
#include "liquid/scene/Camera.h"
#include "liquid/scene/Light.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

TEST(SceneTest, CreatesEmptyRootNodeOnConstruct) {
  liquid::EntityContext context;
  liquid::Scene scene(context);

  auto *rootNode = scene.getRootNode();

  scene.update();

  EXPECT_NE(rootNode->getEntity(), std::numeric_limits<liquid::Entity>::max());
  EXPECT_EQ(rootNode->getParent(), nullptr);
  EXPECT_TRUE(rootNode->getWorldTransform() == glm::mat4{1.0f});
}

TEST(SceneTest, SetsActiveCameraEntity) {
  TestResourceAllocator resourceAllocator;

  liquid::EntityContext context;
  liquid::Scene scene(context);
  auto entity = context.createEntity();
  context.setComponent<liquid::CameraComponent>(
      entity, {std::make_shared<liquid::Camera>(&resourceAllocator)});
  scene.setActiveCamera(entity);
  EXPECT_EQ(scene.getActiveCamera().get(),
            context.getComponent<liquid::CameraComponent>(entity).camera.get());
}

TEST(SceneDeathTest,
     SetingActiveCameraEntityFailsIfEntityHasNoCameraComponent) {
  TestResourceAllocator resourceAllocator;

  liquid::EntityContext context;
  liquid::Scene scene(context);
  EXPECT_DEATH(scene.setActiveCamera(context.createEntity()), ".*");
}
