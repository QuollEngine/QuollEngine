#pragma once

#include "liquid/core/Base.h"
#include "liquid/core/Property.h"
#include "Shader.h"
#include "Texture.h"
#include "HardwareBuffer.h"
#include "ResourceAllocator.h"
#include "MaterialResourceBinder.h"
#include "liquid/renderer/render-graph/RenderGraphPipelineDescriptor.h"
#include "Descriptor.h"

namespace liquid {

class Material {
public:
  /**
   * @brief Creates material
   *
   * @param textures Textures
   * @param properties Material properties
   * @param resourceAllocator Resource allocator
   */
  Material(const std::vector<SharedPtr<Texture>> &textures,
           const std::vector<std::pair<String, Property>> &properties,
           ResourceAllocator *resourceAllocator);

  /**
   * @brief Update property
   *
   * @param name Property name
   * @param property Property value
   */
  void updateProperty(const String &name, const Property &value);

  /**
   * @brief Gets texture
   *
   * @return Pointer to texture
   */
  inline const std::vector<SharedPtr<Texture>> &getTextures() {
    return textures;
  }

  /**
   * @brief Check if there are any textures
   *
   * @retval true Has textures
   * @retval false Does not have textures
   */
  inline bool hasTextures() { return !textures.empty(); }

  /**
   * @brief Gets vertex shader
   *
   * @return Pointer to vertex shader
   */
  inline const SharedPtr<Shader> &getVertexShader() { return vertexShader; }

  /**
   * @brief Gets fragment shader
   *
   * @return Pointer to fragment shader
   */
  inline const SharedPtr<Shader> &getFragmentShader() { return fragmentShader; }

  /**
   * @brief Gets uniform buffer
   *
   * @return Uniform buffer
   */
  inline const SharedPtr<HardwareBuffer> &getUniformBuffer() {
    return uniformBuffer;
  }

  /**
   * @brief Get properties
   *
   * @return Unordered list of properties
   */
  inline const std::vector<Property> &getProperties() const {
    return properties;
  }

  /**
   * @brief Get descriptor
   *
   * @return Descriptor
   */
  inline const Descriptor &getDescriptor() const { return descriptor; }

private:
  /**
   * @brief Copies local data to buffer
   *
   * Merges all properties into a memory region
   * and updates buffer with this memory region
   */
  void updateBufferWithProperties();

private:
  SharedPtr<Shader> vertexShader = nullptr;
  SharedPtr<Shader> fragmentShader = nullptr;
  std::vector<SharedPtr<Texture>> textures;
  SharedPtr<HardwareBuffer> uniformBuffer = nullptr;

  Descriptor descriptor;

  std::vector<Property> properties;
  std::map<String, size_t> propertyMap;
};

} // namespace liquid
