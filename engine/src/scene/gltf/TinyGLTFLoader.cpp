#include "core/Base.h"
#include "core/EngineGlobals.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14

#include <json/json.hpp>
#include <stb/stb_image.h>

#include "scene/Vertex.h"
#include "scene/Mesh.h"
#include "renderer/Texture.h"
#include "renderer/MaterialPBR.h"
#include "TinyGLTFLoader.h"
#include "GLTFError.h"

namespace liquid {

TinyGLTFLoader::BufferMeta
TinyGLTFLoader::getBufferMetaForAccessor(const tinygltf::Model &model,
                                         int accessorIndex) {
  auto accessor = model.accessors.at(accessorIndex);
  auto bufferView = model.bufferViews.at(accessor.bufferView);
  const unsigned char *bufferStart =
      &model.buffers.at(bufferView.buffer).data.at(0);
  const unsigned char *bufferOffset =
      bufferStart + accessor.byteOffset + bufferView.byteOffset;

  return {accessor, bufferView, bufferOffset};
}

TinyGLTFLoader::TinyGLTFLoader(EntityContext &entityContext_,
                               VulkanRenderer *renderer_)
    : entityContext(entityContext_), renderer(renderer_),
      defaultMaterial(renderer->createMaterialPBR({}, CullMode::Back)) {}

SharedPtr<Scene> TinyGLTFLoader::loadFromFile(const String &filename) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  String error, warning;

  bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filename);

  if (!warning.empty()) {
    engineLogger.log(Logger::Warning) << warning;
  }

  if (!error.empty()) {
    throw GLTFError(error);
  }

  if (!ret) {
    throw GLTFError("Failed to parse GLTF file");
  }

  auto &&materials = getMaterials(model);
  auto &&meshes = getMeshes(model, materials);
  auto scene = getScene(model, meshes);

  LOG_DEBUG("[GLTF] Loaded GLTF scene from " << filename);

  return scene;
}

SharedPtr<Scene>
TinyGLTFLoader::getScene(const tinygltf::Model &model,
                         const std::map<uint32_t, Entity> &meshEntityMap) {
  try {
    auto &gltfScene = model.scenes.at(model.defaultScene);
    SharedPtr<Scene> scene(new Scene(entityContext));

    std::vector<SceneNode *> nodes(model.nodes.size());

    std::list<std::pair<int, int>> nodesToProcess;

    for (auto &nodeIndex : gltfScene.nodes) {
      nodesToProcess.push_back(std::make_pair(nodeIndex, -1));
    }

    while (!nodesToProcess.empty()) {
      auto [nodeIndex, parentIndex] = nodesToProcess.front();
      auto &gltfNode = model.nodes.at(nodeIndex);
      nodesToProcess.pop_front();

      Entity entity = gltfNode.mesh >= 0 && meshEntityMap.size() > 0
                          ? meshEntityMap.at(gltfNode.mesh)
                          : entityContext.createEntity();

      entityContext.setComponent<NameComponent>(entity,
                                                {String(gltfNode.name)});

      glm::mat4 transform{1.0f};

      constexpr size_t transformMatrixSize = 6;
      constexpr size_t translationMatrixSize = 3;
      constexpr size_t scaleMatrixSize = 3;
      constexpr size_t rotationMatrixSize = 4;

      if (gltfNode.matrix.size() == transformMatrixSize) {
        transform = glm::make_mat4(gltfNode.matrix.data());
      } else if (gltfNode.matrix.size() > 0) {
        engineLogger.log(Logger::Warning)
            << "Node matrix data must have 16 values. Skipping...";
      } else {
        if (gltfNode.translation.size() == translationMatrixSize) {
          transform *= glm::translate(
              glm::mat4{1.0f},
              glm::vec3(glm::make_vec3(gltfNode.translation.data())));
        } else if (gltfNode.translation.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node translation data must have 3 values. Skipping...";
        }

        if (gltfNode.rotation.size() == rotationMatrixSize) {
          transform *= glm::mat4_cast(
              glm::quat(glm::make_quat(gltfNode.rotation.data())));
        } else if (gltfNode.rotation.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node rotation data must have 4 values. Skipping...";
        }

        if (gltfNode.scale.size() == scaleMatrixSize) {
          transform *= glm::scale(
              glm::mat4{1.0}, glm::vec3(glm::make_vec3(gltfNode.scale.data())));
        } else if (gltfNode.scale.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node scale data must have 3 values. Skipping...";
        }
      }

      if (parentIndex >= 0) {
        nodes.at(nodeIndex) = nodes[parentIndex]->addChild(entity, transform);
      } else {
        nodes.at(nodeIndex) = scene->getRootNode()->addChild(entity, transform);
      }

      for (auto child : gltfNode.children) {
        nodesToProcess.push_back(std::make_pair(child, nodeIndex));
      }
    }

    return scene;
  } catch (std::out_of_range e) {
    throw GLTFError("Failed to load scene");
  }
}

std::map<uint32_t, Entity>
TinyGLTFLoader::getMeshes(const tinygltf::Model &model,
                          const std::vector<SharedPtr<Material>> &materials) {
  std::vector<SharedPtr<MeshInstance>> instances;
  std::map<uint32_t, Entity> entityMap;

  try {
    for (auto i = 0; i < model.meshes.size(); ++i) {
      auto &gltfMesh = model.meshes[i];

      // TODO: Support multiple primitives
      if (gltfMesh.primitives.empty()) {
        engineLogger.log(Logger::Warning)
            << "Mesh #" << i << " does not have primitives. Skipping...";
        continue;
      }

      liquid::Mesh *mesh = new liquid::Mesh;
      for (size_t p = 0; p < gltfMesh.primitives.size(); ++p) {
        auto &primitive = gltfMesh.primitives[p];

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        if (primitive.attributes.find("POSITION") ==
            primitive.attributes.end()) {
          engineLogger.log(Logger::Warning)
              << "Mesh #" << i << ", Primitive #" << p
              << " does not have a position attribute. Skipping...";
          continue;
        }

        if (primitive.indices >= 0) {
          auto &&indexMeta = TinyGLTFLoader::getBufferMetaForAccessor(
              model, primitive.indices);
          indices.resize(indexMeta.accessor.count);
          if (indexMeta.accessor.componentType ==
                  TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT &&
              indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
            const auto *data =
                reinterpret_cast<const uint32_t *>(indexMeta.rawData);
            for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
              indices[i] = data[i];
            }
          } else if (indexMeta.accessor.componentType ==
                         TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT &&
                     indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
            const auto *data =
                reinterpret_cast<const uint16_t *>(indexMeta.rawData);
            for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
              indices[i] = data[i];
            }
          } else if (indexMeta.accessor.componentType ==
                         TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE &&
                     indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
            const auto *data =
                reinterpret_cast<const uint8_t *>(indexMeta.rawData);
            for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
              indices[i] = data[i];
            }
          } else {
            engineLogger.log(Logger::Warning)
                << "Mesh #" << i << ", Primitive #" << p
                << " has invalid index format. Skipping...";
            continue;
          }
        }

        auto &&positionMeta = TinyGLTFLoader::getBufferMetaForAccessor(
            model, primitive.attributes.at("POSITION"));

        size_t vertexSize = positionMeta.accessor.count;
        vertices.resize(vertexSize);

        // According to spec, position attribute can only be vec3<float>
        if (positionMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
            positionMeta.accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_FLOAT) {
          auto *data =
              reinterpret_cast<const glm::vec3 *>(positionMeta.rawData);
          for (size_t i = 0; i < vertexSize; ++i) {
            vertices[i].x = data[i].x;
            vertices[i].y = data[i].y;
            vertices[i].z = data[i].z;
          }
        } else {
          engineLogger.log(Logger::Warning)
              << "Mesh #" << i << ", Primitive #0"
              << " has invalid position format. Skipping...";
          continue;
        }

        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
          auto &&normalMeta = TinyGLTFLoader::getBufferMetaForAccessor(
              model, primitive.attributes.at("NORMAL"));
          // According to spec, normal attribute can only be vec3<float> and
          // all attributes of a primitive must have the same number of items
          if (normalMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
              normalMeta.accessor.componentType ==
                  TINYGLTF_COMPONENT_TYPE_FLOAT &&
              normalMeta.accessor.count == vertexSize) {
            auto *data =
                reinterpret_cast<const glm::vec3 *>(normalMeta.rawData);
            for (size_t i = 0; i < vertexSize; ++i) {
              vertices[i].nx = data[i].x;
              vertices[i].ny = data[i].y;
              vertices[i].nz = data[i].z;
            }
          }
        } else {
          engineLogger.log(Logger::Warning)
              << "Calculating flat normals is not supported";
          // TODO: Calculate flat normals
        }

        if (primitive.attributes.find("TANGENT") !=
            primitive.attributes.end()) {
          auto &&tangentMeta = TinyGLTFLoader::getBufferMetaForAccessor(
              model, primitive.attributes.at("TANGENT"));
          // According to spec, normal attribute can only be vec4<float> and
          // all attributes of a primitive must have the same number of items
          if (tangentMeta.accessor.type == TINYGLTF_TYPE_VEC4 &&
              tangentMeta.accessor.componentType ==
                  TINYGLTF_COMPONENT_TYPE_FLOAT &&
              tangentMeta.accessor.count == vertexSize) {
            auto *data =
                reinterpret_cast<const glm::vec4 *>(tangentMeta.rawData);
            for (size_t i = 0; i < vertexSize; ++i) {
              vertices[i].tx = data[i].x;
              vertices[i].ty = data[i].y;
              vertices[i].tz = data[i].z;
              vertices[i].tw = data[i].w;
            }
          }
        } else {
          // TODO: Calculate tangents using MikkTSpace algorithms
          engineLogger.log(Logger::Warning)
              << "Tangents will be calculated using derivative functions in "
                 "pixel shader. For more accurate results, you need to provide "
                 "the tangent attribute when generating GLTF model.";
        }

        if (primitive.attributes.find("TEXCOORD_0") !=
            primitive.attributes.end()) {
          auto &&uvMeta = TinyGLTFLoader::getBufferMetaForAccessor(
              model, primitive.attributes.at("TEXCOORD_0"));

          // According to spec, UV data is always in vec2 format
          // and all attributes of a primitive must have
          // the same number of items
          if (uvMeta.accessor.type == TINYGLTF_TYPE_VEC2 &&
              uvMeta.accessor.count == vertexSize) {
            // UV coordinates can be float, ubyte, and ushort
            if (uvMeta.accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_FLOAT) {
              auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
              for (size_t i = 0; i < vertexSize; ++i) {
                vertices[i].u0 = data[i].x;
                vertices[i].v0 = data[i].y;
              }
            } else if (uvMeta.accessor.componentType ==
                           TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                       uvMeta.accessor.componentType ==
                           TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
              // TODO: Convert integer coordinates to float
              engineLogger.log(Logger::Warning)
                  << "Integer based texture coordinates are not supported for "
                     "TEXCOORD0";
            }
          }
        }

        if (primitive.attributes.find("TEXCOORD_1") !=
            primitive.attributes.end()) {
          auto &&uvMeta = TinyGLTFLoader::getBufferMetaForAccessor(
              model, primitive.attributes.at("TEXCOORD_1"));

          // According to spec, UV data is always in vec2 format
          // and all attributes of a primitive must have
          // the same number of items
          if (uvMeta.accessor.type == TINYGLTF_TYPE_VEC2 &&
              uvMeta.accessor.count == vertexSize) {
            // UV coordinates can be float, ubyte, and ushort
            if (uvMeta.accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_FLOAT) {
              auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
              for (size_t i = 0; i < vertexSize; ++i) {
                vertices[i].u1 = data[i].x;
                vertices[i].v1 = data[i].y;
              }
            } else if (uvMeta.accessor.componentType ==
                           TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                       uvMeta.accessor.componentType ==
                           TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
              // TODO: Convert integer coordinates to float
              engineLogger.log(Logger::Warning)
                  << "Integer based texture coordinates are not supported for "
                     "TEXCOORD1";
            }
          }
        }

        SharedPtr<Material> material =
            gltfMesh.primitives.at(p).material >= 0
                ? materials.at(gltfMesh.primitives.at(p).material)
                : defaultMaterial;

        liquid::Geometry geometry(vertices, indices, material);
        mesh->addGeometry(geometry);
      }

      auto entity = entityContext.createEntity();
      entityContext.setComponent<MeshComponent>(
          entity, {std::make_shared<MeshInstance>(
                      mesh, renderer->getResourceAllocator())});

      entityMap.insert({i, entity});

      delete mesh;
    }
    return entityMap;
  } catch (std::out_of_range e) {
    throw GLTFError("Failed load meshes");
  }
}

std::vector<SharedPtr<Material>>
TinyGLTFLoader::getMaterials(const tinygltf::Model &model) {
  try {
    std::vector<SharedPtr<Texture>> textures;
    std::vector<SharedPtr<Material>> materials;

    for (auto &gltfTexture : model.textures) {
      // TODO: Support creating different samplers

      auto &image = model.images.at(gltfTexture.source);

      TextureData imageData;
      imageData.height = image.height;
      imageData.width = image.width;
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
      imageData.data = const_cast<unsigned char *>(image.image.data());

      textures.push_back(
          renderer->getResourceAllocator()->createTexture2D(imageData));
    }

    for (auto &gltfMaterial : model.materials) {
      std::vector<std::pair<String, Property>> materialProperties;

      MaterialPBR::Properties properties{};

      if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
        properties.baseColorTexture = textures.at(
            gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
      }
      properties.baseColorTextureCoord =
          gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
      auto &colorFactor = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
      properties.baseColorFactor = glm::vec4{colorFactor[0], colorFactor[1],
                                             colorFactor[2], colorFactor[3]};

      if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >=
          0) {
        properties.metallicRoughnessTexture = textures.at(
            gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index);
      }
      properties.metallicRoughnessTextureCoord =
          gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
      properties.metallicFactor =
          static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
      properties.roughnessFactor =
          static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);

      if (gltfMaterial.normalTexture.index >= 0) {
        properties.normalTexture =
            textures.at(gltfMaterial.normalTexture.index);
      }
      properties.normalTextureCoord = gltfMaterial.normalTexture.texCoord;
      properties.normalScale =
          static_cast<float>(gltfMaterial.normalTexture.scale);

      if (gltfMaterial.occlusionTexture.index >= 0) {
        properties.occlusionTexture =
            textures.at(gltfMaterial.occlusionTexture.index);
      }
      properties.occlusionTextureCoord = gltfMaterial.occlusionTexture.texCoord;
      properties.occlusionStrength =
          static_cast<float>(gltfMaterial.occlusionTexture.strength);

      if (gltfMaterial.emissiveTexture.index >= 0) {
        properties.emissiveTexture =
            textures.at(gltfMaterial.emissiveTexture.index);
      }
      properties.emissiveTextureCoord = gltfMaterial.emissiveTexture.texCoord;
      auto &emissiveFactor = gltfMaterial.emissiveFactor;
      properties.emissiveFactor =
          glm::vec3{emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]};

      CullMode cullMode =
          gltfMaterial.doubleSided ? CullMode::None : CullMode::Back;
      materials.push_back(renderer->createMaterialPBR(properties, cullMode));
    }

    return materials;
  } catch (std::out_of_range e) {
    throw GLTFError("Failed load textures and materials");
  }
}

} // namespace liquid
