#include "liquid/core/Base.h"
#include "liquid/scene/Camera.h"

#include <gtest/gtest.h>

class CameraTest : public ::testing::Test {
public:
  liquid::experimental::ResourceRegistry registry;
};

TEST_F(CameraTest, CreatesUniformBufferOnConstruct) {
  liquid::Camera camera(&registry);

  EXPECT_EQ(
      registry.getBufferMap().getDescription(camera.getUniformBuffer()).type,
      liquid::BufferType::Uniform);
}

TEST_F(CameraTest, SetsPerspectiveProjectionAndUpdatesProjectionView) {
  liquid::Camera camera(&registry);

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionMatrix()) ==
              glm::mat4{1.0f});
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              glm::mat4{1.0f});

  camera.setPerspective(90.0f, 0.5f, 0.1f, 100.0f);

  auto projection = glm::perspective(glm::radians(90.0f), 0.5f, 0.1f, 100.0f);
  projection[1][1] *= -1;

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionMatrix()) ==
              projection);
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              projection);
}

TEST_F(CameraTest, SetsViewAndUpdatesProjectionView) {
  liquid::Camera camera(&registry);

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getViewMatrix()) ==
              glm::mat4{1.0f});
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              glm::mat4{1.0f});

  camera.lookAt({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

  auto view =
      glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getViewMatrix()) == view);
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              view);
}

TEST_F(CameraTest, UpdatesBufferOnSetProjectionView) {
  liquid::Camera camera(&registry);

  EXPECT_EQ(
      registry.getBufferMap().getDescription(camera.getUniformBuffer()).data,
      nullptr);

  camera.setPerspective(90.0f, 0.5f, 0.1f, 100.0f);

  auto *data =
      registry.getBufferMap().getDescription(camera.getUniformBuffer()).data;

  EXPECT_NE(data, nullptr);
  auto &cameraData = *static_cast<liquid::CameraData *>(data);
  auto &realCameraData = camera.getCameraData();
  EXPECT_TRUE(cameraData.projectionMatrix == realCameraData.projectionMatrix);
  EXPECT_TRUE(cameraData.viewMatrix == realCameraData.viewMatrix);
  EXPECT_TRUE(cameraData.projectionViewMatrix ==
              realCameraData.projectionViewMatrix);
}

TEST_F(CameraTest, UpdatesBufferOnLookAt) {
  liquid::Camera camera(&registry);

  EXPECT_EQ(
      registry.getBufferMap().getDescription(camera.getUniformBuffer()).data,
      nullptr);

  camera.lookAt({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

  auto *data =
      registry.getBufferMap().getDescription(camera.getUniformBuffer()).data;

  EXPECT_NE(data, nullptr);

  auto &cameraData = *static_cast<liquid::CameraData *>(data);
  auto &realCameraData = camera.getCameraData();
  EXPECT_TRUE(cameraData.projectionMatrix == realCameraData.projectionMatrix);
  EXPECT_TRUE(cameraData.viewMatrix == realCameraData.viewMatrix);
  EXPECT_TRUE(cameraData.projectionViewMatrix ==
              realCameraData.projectionViewMatrix);
}
