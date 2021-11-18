#include "core/Base.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/Light.h"

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

TEST(SceneTest, SetsActiveCamera) {
  TestResourceAllocator resourceAllocator;

  liquid::EntityContext context;
  liquid::Scene scene(context);
  std::unique_ptr<liquid::Camera> camera(
      new liquid::Camera(&resourceAllocator));
  scene.setActiveCamera(camera.get());
  EXPECT_EQ(scene.getActiveCamera(), camera.get());
}
