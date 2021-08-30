#pragma once

#include "core/Base.h"
#include "core/Property.h"
#include "Shader.h"
#include "Texture.h"
#include "HardwareBuffer.h"
#include "ResourceAllocator.h"
#include "MaterialResourceBinder.h"
#include "ResourceManager.h"
#include "CullMode.h"

namespace liquid {

class Material {
public:
  /**
   * @brief Creates material
   *
   * @param vertexShader Vertex shader
   * @param fragmentShader Fragment shader
   * @param textures Textures
   * @param properties Material properties
   * @param cullMode Cull mode
   * @param resourceAllocator Resource allocator
   * @param resourceManager Resource manager
   */
  Material(const SharedPtr<Shader> &vertexShader,
           const SharedPtr<Shader> &fragmentShader,
           const std::vector<SharedPtr<Texture>> &textures,
           const std::vector<std::pair<String, Property>> &properties,
           const CullMode &cullMode, ResourceAllocator *resourceAllocator,
           ResourceManager *resourceManager);

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
   * @brief Get resource binder
   *
   * @return Resource binder
   */
  inline const SharedPtr<MaterialResourceBinder> &getResourceBinder() {
    return resourceBinder;
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
   * @brief Get cull mode
   *
   * @return Cull mode
   */
  inline const CullMode &getCullMode() const { return cullMode; }

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
  CullMode cullMode;

  std::vector<Property> properties;
  std::map<String, size_t> propertyMap;

  SharedPtr<MaterialResourceBinder> resourceBinder = nullptr;
};

} // namespace liquid
