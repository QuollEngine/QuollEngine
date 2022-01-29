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

/**
 * @brief Decomposes matrix into TRS values
 *
 * @param matrix Input matrix
 * @param position Output position
 * @param rotation Output rotation
 * @param scale Output scale
 */
static void decomposeMatrix(const glm::mat4 &matrix, glm::vec3 &position,
                            glm::quat &rotation, glm::vec3 &scale) {
  glm::mat4 temp = matrix;
  position = temp[3];

  for (glm::mat4::length_type i = 0; i < 3; ++i) {
    scale[i] = glm::length(temp[i]);
    temp[i] /= scale[i];
  }

  rotation = glm::toQuat(matrix);
}

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
                               VulkanRenderer *renderer_,
                               AnimationSystem &animationSystem_)
    : entityContext(entityContext_), renderer(renderer_),
      animationSystem(animationSystem_),
      defaultMaterial(renderer->createMaterialPBR({}, CullMode::Back)) {}

SceneNode *TinyGLTFLoader::loadFromFile(const String &filename) {
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

  auto &&animations = getAnimations(model);
  auto &&materials = getMaterials(model);
  auto &&meshes = getMeshes(model, materials);
  auto *scene = getScene(model, meshes, animations);

  LOG_DEBUG("[GLTF] Loaded GLTF scene from " << filename);

  return scene;
}

SceneNode *
TinyGLTFLoader::getScene(const tinygltf::Model &model,
                         const std::map<uint32_t, Entity> &meshEntityMap,
                         const std::map<uint32_t, String> &nodeAnimationMap) {
  try {
    auto &gltfScene = model.scenes.at(model.defaultScene);
    auto *rootNode =
        new SceneNode(entityContext.createEntity(), {}, nullptr, entityContext);

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

      String nodeName = String(gltfNode.name);
      String entityName =
          nodeName.length() > 0 ? nodeName : "Entity " + std::to_string(entity);
      entityContext.setComponent<NameComponent>(entity, {entityName});

      auto animation = nodeAnimationMap.find(nodeIndex);
      if (animation != nodeAnimationMap.end()) {
        entityContext.setComponent<AnimationComponent>(entity,
                                                       {animation->second});
      }

      constexpr size_t TRANSFORM_MATRIX_SIZE = 6;
      constexpr size_t TRANSLATION_MATRIX_SIZE = 3;
      constexpr size_t SCALE_MATRIX_SIZE = 3;
      constexpr size_t ROTATION_MATRIX_SIZE = 4;

      TransformComponent transform;

      if (gltfNode.matrix.size() == TRANSFORM_MATRIX_SIZE) {
        decomposeMatrix(glm::make_mat4(gltfNode.matrix.data()),
                        transform.localPosition, transform.localRotation,
                        transform.localScale);

      } else if (gltfNode.matrix.size() > 0) {
        engineLogger.log(Logger::Warning)
            << "Node matrix data must have 16 values. Skipping...";
      } else {
        if (gltfNode.translation.size() == TRANSLATION_MATRIX_SIZE) {
          transform.localPosition = glm::make_vec3(gltfNode.translation.data());
        } else if (gltfNode.translation.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node translation data must have 3 values. Skipping...";
        }

        if (gltfNode.rotation.size() == ROTATION_MATRIX_SIZE) {
          transform.localRotation = glm::make_quat(gltfNode.rotation.data());
        } else if (gltfNode.rotation.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node rotation data must have 4 values. Skipping...";
        }

        if (gltfNode.scale.size() == SCALE_MATRIX_SIZE) {
          transform.localScale = glm::make_vec3(gltfNode.scale.data());
        } else if (gltfNode.scale.size() > 0) {
          engineLogger.log(Logger::Warning)
              << "Node scale data must have 3 values. Skipping...";
        }
      }

      if (parentIndex >= 0) {
        nodes.at(nodeIndex) = nodes[parentIndex]->addChild(entity, transform);
      } else {
        nodes.at(nodeIndex) = rootNode->addChild(entity, transform);
      }

      for (auto child : gltfNode.children) {
        nodesToProcess.push_back(std::make_pair(child, nodeIndex));
      }
    }

    return rootNode;
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

std::map<uint32_t, String>
TinyGLTFLoader::getAnimations(const tinygltf::Model &model) {
  std::map<uint32_t, String> animations;
  for (size_t i = 0; i < model.animations.size(); ++i) {
    const auto &gltfAnimation = model.animations.at(i);

    struct SamplerInfo {
      std::vector<float> times;
      std::vector<glm::vec4> values;
      KeyframeSequenceInterpolation interpolation =
          KeyframeSequenceInterpolation::Linear;
    };

    std::vector<SamplerInfo> samplers(gltfAnimation.samplers.size());

    float maxTime = 0.0f;

    for (size_t i = 0; i < gltfAnimation.samplers.size(); ++i) {
      const auto &sampler = gltfAnimation.samplers.at(i);
      const auto &input =
          TinyGLTFLoader::getBufferMetaForAccessor(model, sampler.input);
      const auto &output =
          TinyGLTFLoader::getBufferMetaForAccessor(model, sampler.output);

      LIQUID_ASSERT(input.accessor.type == TINYGLTF_TYPE_SCALAR,
                    "Animation time accessor must be in SCALAR format");

      LIQUID_ASSERT(input.accessor.componentType ==
                        TINYGLTF_COMPONENT_TYPE_FLOAT,
                    "Animation time accessor component type must be FLOAT");

      LIQUID_ASSERT(
          input.accessor.count == output.accessor.count,
          "Sampler input and output must have the same number of items");

      LIQUID_ASSERT(output.accessor.componentType ==
                        TINYGLTF_COMPONENT_TYPE_FLOAT,
                    "Animation output accessor component type must be FLOAT");

      std::vector<float> &times = samplers.at(i).times;
      times.resize(input.accessor.count);

      std::vector<glm::vec4> &values = samplers.at(i).values;
      values.resize(output.accessor.count);

      if (sampler.interpolation == "LINEAR") {
        samplers.at(i).interpolation = KeyframeSequenceInterpolation::Linear;
      } else if (sampler.interpolation == "STEP") {
        samplers.at(i).interpolation = KeyframeSequenceInterpolation::Step;
      }

      float max = 0.0f;

      {
        const float *inputData = reinterpret_cast<const float *>(input.rawData);
        for (size_t i = 0; i < input.accessor.count; ++i) {
          times.at(i) = inputData[i];
          max = std::max(max, inputData[i]);
        }
      }

      maxTime = std::max(max, maxTime);

      // Normalize the time
      {
        for (size_t i = 0; i < times.size(); ++i) {
          times.at(i) = times.at(i) / max;
        }
      }

      if (output.accessor.type == TINYGLTF_TYPE_VEC3) {
        const glm::vec3 *outputData =
            reinterpret_cast<const glm::vec3 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0.0f);
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_VEC4) {
        const glm::vec4 *outputData =
            reinterpret_cast<const glm::vec4 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = outputData[i];
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_SCALAR) {
        const float *outputData =
            reinterpret_cast<const float *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0, 0, 0);
        }
      }
    }

    Animation animation(gltfAnimation.name, maxTime);
    int32_t targetNode = -1;

    for (const auto &channel : gltfAnimation.channels) {
      const auto &sampler = samplers.at(channel.sampler);

      if (channel.target_node == -1) {
        // Ignore channel if target node is not specified
        continue;
      }

      if (targetNode == -1) {
        targetNode = channel.target_node;
      } else {
        LIQUID_ASSERT(
            targetNode == channel.target_node,
            "All channels in animation must point to the same target node");
      }

      KeyframeSequenceTarget target = KeyframeSequenceTarget::Position;
      if (channel.target_path == "rotation") {
        target = KeyframeSequenceTarget::Rotation;
      } else if (channel.target_path == "scale") {
        target = KeyframeSequenceTarget::Scale;
      } else if (channel.target_path == "position") {
        target = KeyframeSequenceTarget::Position;
      }

      KeyframeSequence sequence(target, sampler.interpolation);
      for (size_t i = 0; i < sampler.times.size(); ++i) {
        sequence.addKeyframe(sampler.times.at(i), sampler.values.at(i));
      }
      animation.addKeyframeSequence(sequence);
    }

    LIQUID_ASSERT(targetNode >= 0, "Animation must have a target node");

    animations.insert({targetNode, animation.getName()});
    animationSystem.addAnimation(animation);
  }
  return animations;
}

} // namespace liquid
