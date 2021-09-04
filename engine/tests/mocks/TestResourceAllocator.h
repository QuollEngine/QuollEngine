#pragma once

#include "renderer/ResourceAllocator.h"
#include "TestBuffer.h"
#include "TestTextureResourceBinder.h"

class TestResourceAllocator : public liquid::ResourceAllocator {
public:
  virtual liquid::HardwareBuffer *createVertexBuffer(size_t vertexSize) {
    return new TestBuffer(vertexSize);
  }

  virtual liquid::HardwareBuffer *
  createVertexBuffer(const std::vector<liquid::Vertex> &vertices) {
    return new TestBuffer(vertices);
  }

  virtual liquid::HardwareBuffer *createIndexBuffer(size_t indexBuffer) {
    return new TestBuffer(indexBuffer);
  }

  virtual liquid::HardwareBuffer *
  createIndexBuffer(const std::vector<uint32_t> &indices) {
    return new TestBuffer(indices);
  }

  virtual liquid::HardwareBuffer *createUniformBuffer(size_t bufferSize) {
    return new TestBuffer(bufferSize);
  }

  virtual liquid::HardwareBuffer *createMaterialBuffer(size_t bufferSize) {
    return new TestBuffer(bufferSize);
  };

  liquid::SharedPtr<liquid::Texture> virtual createTexture2D(
      const liquid::TextureData &texture) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::Texture2D;
    binder->width = texture.width;
    binder->height = texture.width;
    binder->data = texture.data;
    return std::make_shared<liquid::Texture>(
        binder, texture.width * texture.height * 4, nullptr);
  }

  liquid::SharedPtr<liquid::Texture> virtual createTextureCubemap(
      const liquid::TextureCubemapData &texture) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureCubemap;
    binder->width = texture.width;
    binder->height = texture.height;
    binder->data = texture.data;
    return std::make_shared<liquid::Texture>(
        binder, texture.width * texture.height * 6 * 4, nullptr);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTextureShadowmap(uint32_t dimensions, uint32_t layers) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureShadowmap;
    binder->width = dimensions;
    binder->height = dimensions;
    binder->data = nullptr;
    return std::make_shared<liquid::Texture>(binder, dimensions * dimensions,
                                             nullptr);
  }
};
