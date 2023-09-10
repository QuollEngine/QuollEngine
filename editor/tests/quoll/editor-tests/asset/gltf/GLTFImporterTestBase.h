#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/asset/DefaultObjects.h"
#include "quoll/rhi-mock/MockRenderDevice.h"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14
#include <tiny_gltf.h>

namespace fs = std::filesystem;

static const quoll::Path CachePath = fs::current_path() / "cache";
static const quoll::Path TempPath = fs::current_path() / "temp2";

enum class GLTFTestAttribute {
  Positions,
  Normals,
  Tangents,
  TexCoords0,
  TexCoords1,
  Indices
};

template <class T> struct GLTFTestBufferData {
  int componentType;
  int type;
  std::vector<T> data;

  void fillWithZeros(size_t size) {
    data.resize(size);
    for (size_t i = 0; i < size; ++i) {
      data.at(i) = T{0};
    }
  }
};

struct GLTFTestPrimitive {
  template <class T> GLTFTestBufferData<T> &a(const quoll::String &name) {
    return GLTFTestBufferData<T>{};
  }

  template <GLTFTestAttribute T> constexpr auto &get() {
    if constexpr (T == GLTFTestAttribute::Positions) {
      return positions;
    }

    if constexpr (T == GLTFTestAttribute::Normals) {
      return normals;
    }

    if constexpr (T == GLTFTestAttribute::Tangents) {
      return tangents;
    }

    if constexpr (T == GLTFTestAttribute::TexCoords0) {
      return texCoords0;
    }

    if constexpr (T == GLTFTestAttribute::TexCoords1) {
      return texCoords1;
    }

    if constexpr (T == GLTFTestAttribute::Indices) {
      return indices;
    }
  }

  GLTFTestBufferData<glm::vec3> positions{TINYGLTF_COMPONENT_TYPE_FLOAT,
                                          TINYGLTF_TYPE_VEC3};
  GLTFTestBufferData<glm::vec3> normals{TINYGLTF_COMPONENT_TYPE_FLOAT,
                                        TINYGLTF_TYPE_VEC3};
  GLTFTestBufferData<glm::vec4> tangents{TINYGLTF_COMPONENT_TYPE_FLOAT,
                                         TINYGLTF_TYPE_VEC4};
  GLTFTestBufferData<glm::vec2> texCoords0{TINYGLTF_COMPONENT_TYPE_FLOAT,
                                           TINYGLTF_TYPE_VEC2};
  GLTFTestBufferData<glm::vec2> texCoords1{TINYGLTF_COMPONENT_TYPE_FLOAT,
                                           TINYGLTF_TYPE_VEC2};
  GLTFTestBufferData<uint32_t> indices{TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
                                       TINYGLTF_TYPE_SCALAR};
};

struct GLTFTestMesh {
  std::vector<GLTFTestPrimitive> primitives;
};

struct GLTFTestNode {
  glm::vec3 position;
  glm::vec3 scale;

  std::vector<int> children;
};

struct GLTFTestSkin {
  GLTFTestBufferData<glm::mat4> inverseBindMatrices{
      TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_MAT4};

  std::vector<int> joints;
};

struct GLTFTestScene {
  std::vector<GLTFTestMesh> meshes;
  std::vector<GLTFTestNode> nodes;
  std::vector<GLTFTestSkin> skins;
};

template <class T>
int createBufferFromVector(tinygltf::Model &model,
                           GLTFTestBufferData<T> &bufferData) {
  size_t size = sizeof(T) * bufferData.data.size();

  tinygltf::Buffer buffer;
  buffer.data.resize(size);
  auto *rawData = &buffer.data.at(0);
  memcpy(rawData, bufferData.data.data(), size);

  model.buffers.push_back(buffer);

  auto index = static_cast<int>(model.buffers.size() - 1);

  tinygltf::BufferView view;
  view.buffer = index;
  view.byteOffset = 0;
  view.byteLength = size;

  model.bufferViews.push_back(view);

  tinygltf::Accessor accessor;
  accessor.componentType = bufferData.componentType;
  accessor.type = bufferData.type;
  accessor.bufferView = static_cast<int>(model.bufferViews.size() - 1);
  accessor.byteOffset = 0;
  accessor.count = bufferData.data.size();

  model.accessors.push_back(accessor);

  return static_cast<int>(model.accessors.size() - 1);
}

static quoll::Path saveGLTF(const quoll::String &filename,
                            tinygltf::Model &model) {
  tinygltf::TinyGLTF gltf;
  gltf.WriteGltfSceneToFile(&model, (TempPath / filename).string(), true, true,
                            true, true);

  return TempPath / filename;
}

static tinygltf::Node createNode(tinygltf::Model &model, GLTFTestNode &inNode) {
  tinygltf::Node node;
  node.translation.resize(3);
  node.translation.at(0) = inNode.position.x;
  node.translation.at(1) = inNode.position.y;
  node.translation.at(2) = inNode.position.z;

  node.scale.resize(3);
  node.scale.at(0) = inNode.scale.x;
  node.scale.at(1) = inNode.scale.y;
  node.scale.at(2) = inNode.scale.z;

  node.children = inNode.children;
  return node;
}

static tinygltf::Skin createSkin(tinygltf::Model &model, GLTFTestSkin &inSkin) {
  tinygltf::Skin skin;
  skin.inverseBindMatrices =
      createBufferFromVector(model, inSkin.inverseBindMatrices);

  skin.joints.reserve(inSkin.joints.size());

  for (auto joint : inSkin.joints) {
    skin.joints.push_back(joint);
  }

  return skin;
}

static tinygltf::Primitive createPrimitive(tinygltf::Model &model,
                                           GLTFTestPrimitive &inPrimitive) {
  constexpr size_t FloatSize = sizeof(float);

  tinygltf::Primitive primitive;
  primitive.mode = TINYGLTF_MODE_TRIANGLES;

  if (!inPrimitive.positions.data.empty()) {
    primitive.attributes["POSITION"] =
        createBufferFromVector(model, inPrimitive.positions);
  }

  if (!inPrimitive.normals.data.empty()) {
    primitive.attributes["NORMAL"] =
        createBufferFromVector(model, inPrimitive.normals);
  }

  if (!inPrimitive.tangents.data.empty()) {
    primitive.attributes["TANGENT"] =
        createBufferFromVector(model, inPrimitive.tangents);
  }

  if (!inPrimitive.texCoords0.data.empty()) {
    primitive.attributes["TEXCOORD_0"] =
        createBufferFromVector(model, inPrimitive.texCoords0);
  }

  if (!inPrimitive.texCoords1.data.empty()) {
    primitive.attributes["TEXCOORD_1"] =
        createBufferFromVector(model, inPrimitive.texCoords1);
  }

  if (!inPrimitive.indices.data.empty()) {
    if (inPrimitive.indices.componentType ==
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
      GLTFTestBufferData<uint16_t> indices{
          TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR};
      indices.data.resize(inPrimitive.indices.data.size());
      for (size_t i = 0; i < indices.data.size(); ++i) {
        indices.data.at(i) =
            static_cast<uint16_t>(inPrimitive.indices.data.at(i));
      }

      primitive.indices = createBufferFromVector(model, indices);
    } else if (inPrimitive.indices.componentType ==
               TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
      GLTFTestBufferData<uint8_t> indices{TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                                          TINYGLTF_TYPE_SCALAR};
      indices.data.resize(inPrimitive.indices.data.size());
      for (size_t i = 0; i < indices.data.size(); ++i) {
        indices.data.at(i) =
            static_cast<uint8_t>(inPrimitive.indices.data.at(i));
      }

      primitive.indices = createBufferFromVector(model, indices);

    } else {
      primitive.indices = createBufferFromVector(model, inPrimitive.indices);
    }
  }

  if (primitive.attributes.empty()) {
    GLTFTestBufferData<int> data{TINYGLTF_COMPONENT_TYPE_INT,
                                 TINYGLTF_TYPE_SCALAR};
    data.data.push_back(0);
    primitive.attributes["_TEST_DO_NOT_USE"] =
        createBufferFromVector(model, data);
  }

  return primitive;
}

static GLTFTestPrimitive createCubePrimitive() {
  auto asset = quoll::default_objects::createCube();
  const auto &g = asset.data.geometries.at(0);

  GLTFTestPrimitive primitive;
  primitive.positions.data.resize(g.positions.size());
  primitive.normals.data.resize(g.positions.size());
  primitive.tangents.data.resize(g.positions.size());
  primitive.texCoords0.data.resize(g.positions.size());
  primitive.texCoords1.data.resize(g.positions.size());
  primitive.indices.data.resize(g.indices.size());

  for (size_t i = 0; i < g.positions.size(); ++i) {
    primitive.positions.data.at(i) = g.positions.at(i);
    primitive.normals.data.at(i) = g.normals.at(i);
    primitive.tangents.data.at(i) = g.tangents.at(i);
    primitive.texCoords0.data.at(i) = g.texCoords0.at(i);
    primitive.texCoords1.data.at(i) = g.texCoords1.at(i);
  }

  for (size_t i = 0; i < g.indices.size(); ++i) {
    primitive.indices.data.at(i) = g.indices.at(i);
  }

  return primitive;
}

static tinygltf::Mesh createMesh(tinygltf::Model &model, GLTFTestMesh &inMesh) {
  constexpr size_t FloatSize = sizeof(float);

  tinygltf::Mesh mesh;

  for (auto &p : inMesh.primitives) {
    mesh.primitives.push_back(createPrimitive(model, p));
  }

  return mesh;
}

static quoll::Path saveSceneGLTF(GLTFTestScene &scene) {
  tinygltf::Scene gltfScene;
  gltfScene.name = "Scene";
  tinygltf::Model model;
  model.asset.version = "2.0";
  model.asset.generator = "tinygltf";
  model.scenes.push_back(gltfScene);
  model.defaultScene = 0;

  for (auto &mesh : scene.meshes) {
    model.meshes.push_back(createMesh(model, mesh));
  }

  for (auto &node : scene.nodes) {
    model.nodes.push_back(createNode(model, node));
  }

  for (auto &skin : scene.skins) {
    model.skins.push_back(createSkin(model, skin));
  }

  return saveGLTF("output.gltf", model);
}

class GLTFImporterTestBase : public ::testing::Test {
public:
  GLTFImporterTestBase()
      : renderStorage(&device), assetCache(CachePath, false),
        imageLoader(assetCache, renderStorage),
        importer(assetCache, imageLoader, false) {}

  void SetUp() override {
    fs::create_directory(CachePath);
    fs::create_directory(TempPath);
  }

  void TearDown() override {
    fs::remove_all(CachePath);
    fs::remove_all(TempPath);
  }

  quoll::rhi::MockRenderDevice device;
  quoll::RenderStorage renderStorage;
  quoll::AssetCache assetCache;
  quoll::editor::ImageLoader imageLoader;
  quoll::editor::GLTFImporter importer;
};
