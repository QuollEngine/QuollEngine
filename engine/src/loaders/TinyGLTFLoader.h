#pragma once

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14

#include "core/Base.h"
#include "scene/Scene.h"
#include "scene/Mesh.h"

#include "renderer/vulkan/VulkanRenderer.h"

#include <tinygltf/tiny_gltf.h>

namespace liquid {

class TinyGLTFLoader {
public:
  /**
   * @brief Creates loader with shaders and renderer
   *
   * @param entityContext Entity context
   * @param renderer Vulkan renderer
   */
  TinyGLTFLoader(EntityContext &entityContext, VulkanRenderer *renderer);

  /**
   * @brief Load GLTF from ASCII file
   *
   * @param filename File name
   * @return Scene node
   */
  SceneNode *loadFromFile(const String &filename);

private:
  struct BufferMeta {
    tinygltf::Accessor accessor;
    tinygltf::BufferView bufferView;
    const unsigned char *rawData;
  };

private:
  /**
   * @brief Reads mesh data and creates mesh instances
   *
   * Conforms to on GLTF 2.0 spec
   * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
   *
   * @param model TinyGLTF model
   * @param materials List of materials
   * @return Map for mesh index and entity
   */
  std::map<uint32_t, Entity>
  getMeshes(const tinygltf::Model &model,
            const std::vector<SharedPtr<Material>> &materials);

  /**
   * @brief Reads material and texture data and creates materials
   *
   * Conforms to on GLTF 2.0 spec
   * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
   *
   * @param model TinyGLTF model
   * @return List of materials
   */
  std::vector<SharedPtr<Material>> getMaterials(const tinygltf::Model &model);

  /**
   * @brief Reads scene data and creates scenes
   *
   * Conforms to on GLTF 2.0 spec
   * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
   *
   * @param model TinyGLTF model
   * @param meshEntityMap Mesh index entity map
   * @return Scene node
   */
  SceneNode *getScene(const tinygltf::Model &model,
                      const std::map<uint32_t, Entity> &meshEntityMap);

  /**
   * @brief Geta buffer metadata for accessor
   *
   * @param model TinyGLTF model
   * @param accessorIndex Index of buffer accessor
   * @return GLTF buffer metadata
   */
  static BufferMeta getBufferMetaForAccessor(const tinygltf::Model &model,
                                             int accessorIndex);

private:
  EntityContext &entityContext;
  VulkanRenderer *renderer = nullptr;

  SharedPtr<Material> defaultMaterial;
};

} // namespace liquid
