#pragma once

#include "renderer/ResourceAllocator.h"
#include "TestBuffer.h"
#include "TestTextureResourceBinder.h"

class TestResourceAllocator : public liquid::ResourceAllocator {
public:
  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createVertexBuffer(size_t vertexSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::VERTEX,
                                        vertexSize, statsManager);
  }

  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createIndexBuffer(size_t indexSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::INDEX,
                                        indexSize, statsManager);
  }

  virtual liquid::SharedPtr<liquid::HardwareBuffer>
  createUniformBuffer(size_t bufferSize) {
    return std::make_shared<TestBuffer>(liquid::HardwareBuffer::UNIFORM,
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
        binder, texture.width * texture.height * 4, statsManager);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTextureCubemap(const liquid::TextureCubemapData &texture) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureCubemap;
    binder->width = texture.width;
    binder->height = texture.height;
    binder->data = texture.data;
    return std::make_shared<liquid::Texture>(
        binder, texture.width * texture.height * 6 * 4, statsManager);
  }

  virtual liquid::SharedPtr<liquid::Texture>
  createTextureFramebuffer(const liquid::TextureFramebufferData &data) {
    auto binder = std::make_shared<TestTextureResourceBinder>();
    binder->type = TestTextureResourceBinder::TextureFramebuffer;
    binder->width = data.width;
    binder->height = data.height;
    binder->format = data.format;
    binder->data = nullptr;
    return std::make_shared<liquid::Texture>(binder, data.width * data.height,
                                             statsManager);
  }

protected:
  liquid::StatsManager statsManager;
};
