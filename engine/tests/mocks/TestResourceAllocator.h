#pragma once

#include "renderer/ResourceAllocator.h"
#include "TestBuffer.h"
#include "TestTextureResourceBinder.h"

class TestResourceAllocator : public liquid::ResourceAllocator {
public:
  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createVertexBuffer(size_t vertexSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::Vertex,
                                        vertexSize, statsManager);
  }

  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createIndexBuffer(size_t indexSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::Index,
                                        indexSize, statsManager);
  }

  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createUniformBuffer(size_t bufferSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::Uniform,
                                        bufferSize, statsManager);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTexture2D(const liquid::TextureData &texture) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::Texture2D;
    binder->width = texture.width;
    binder->height = texture.width;
    binder->data = texture.data;
    return std::make_shared<liquid::Texture>(
        binder, texture.width * texture.height * 4, texture.width,
        texture.height, 1, texture.format, statsManager);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTextureCubemap(const liquid::TextureCubemapData &texture) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureCubemap;
    binder->width = texture.width;
    binder->height = texture.height;
    binder->data = texture.data;
    return std::make_shared<liquid::Texture>(
        binder, texture.width * texture.height * 6 * 4, texture.width,
        texture.height, 6, texture.format, statsManager);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTextureFramebuffer(const liquid::TextureFramebufferData &data) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureFramebuffer;
    binder->width = data.width;
    binder->height = data.height;
    binder->format = data.format;
    binder->data = nullptr;
    return std::make_shared<liquid::Texture>(
        binder, data.width * data.height, data.width, data.height, data.layers,
        data.format, statsManager);
  }

protected:
  liquid::StatsManager statsManager;
};
