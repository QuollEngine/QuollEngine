#include "core/Base.h"
#include "scene/Camera.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

TEST(CameraTest, CreatesUniformBufferOnConstruct) {
  TestResourceAllocator resourceAllocator;
  liquid::Camera camera(&resourceAllocator);
  EXPECT_EQ(camera.getUniformBuffer()->getType(),
            liquid::HardwareBuffer::Uniform);
}

TEST(CameraTest, SetsPerspectiveProjectionAndUpdatesProjectionView) {
  TestResourceAllocator resourceAllocator;
  liquid::Camera camera(&resourceAllocator);

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionMatrix()) ==
              glm::mat4{1.0f});
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              glm::mat4{1.0f});

  camera.setPerspective(90.0, 0.5, 0.1, 100.0);

  auto projection = glm::perspective(glm::radians(90.0f), 0.5f, 0.1f, 100.0f);
  projection[1][1] *= -1;

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionMatrix()) ==
              projection);
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              projection);
}

TEST(CameraTest, SetsViewAndUpdatesProjectionView) {
  TestResourceAllocator resourceAllocator;
  liquid::Camera camera(&resourceAllocator);

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getViewMatrix()) ==
              glm::mat4{1.0f});
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              glm::mat4{1.0f});

  camera.lookAt({0.0f, 0.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 0.0});

  auto view =
      glm::lookAt(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getViewMatrix()) == view);
  EXPECT_TRUE(const_cast<glm::mat4 &>(camera.getProjectionViewMatrix()) ==
              view);
}

TEST(CameraData, UpdatesBufferOnSetProjectionView) {
  TestResourceAllocator resourceAllocator;
  liquid::Camera camera(&resourceAllocator);

  auto *buffer = static_cast<TestBuffer *>(camera.getUniformBuffer().get());
  EXPECT_EQ(buffer->data, nullptr);

  camera.setPerspective(90.0, 0.5, 0.1, 100.0);

  EXPECT_NE(buffer->data, nullptr);
  auto &cameraData = *static_cast<liquid::CameraData *>(buffer->data);
  auto &realCameraData = camera.getCameraData();
  EXPECT_TRUE(cameraData.projectionMatrix == realCameraData.projectionMatrix);
  EXPECT_TRUE(cameraData.viewMatrix == realCameraData.viewMatrix);
  EXPECT_TRUE(cameraData.projectionViewMatrix ==
              realCameraData.projectionViewMatrix);
}

TEST(CameraData, UpdatesBufferOnLookAt) {
  TestResourceAllocator resourceAllocator;
  liquid::Camera camera(&resourceAllocator);

  auto *buffer = static_cast<TestBuffer *>(camera.getUniformBuffer().get());
  EXPECT_EQ(buffer->data, nullptr);

  camera.lookAt({0.0f, 0.0, 0.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 0.0});

  EXPECT_NE(buffer->data, nullptr);

  auto &cameraData = *static_cast<liquid::CameraData *>(buffer->data);
  auto &realCameraData = camera.getCameraData();
  EXPECT_TRUE(cameraData.projectionMatrix == realCameraData.projectionMatrix);
  EXPECT_TRUE(cameraData.viewMatrix == realCameraData.viewMatrix);
  EXPECT_TRUE(cameraData.projectionViewMatrix ==
              realCameraData.projectionViewMatrix);
}
