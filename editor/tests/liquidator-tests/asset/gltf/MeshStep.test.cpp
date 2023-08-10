#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquidator-tests/Testing.h"
#include "liquidator/asset/GLTFImporter.h"

#include "GLTFImporterTestBase.h"

template <class T> using TestFn = std::function<void(T, T)>;

struct AttributeTestFns {
  TestFn<glm::vec3> testPosition = [](auto p1, auto p2) { EXPECT_EQ(p1, p2); };

  TestFn<glm::vec3> testNormal = [](auto n1, auto n2) { EXPECT_EQ(n1, n2); };

  TestFn<glm::vec4> testTangent = [](auto t1, auto t2) { EXPECT_EQ(t1, t2); };

  TestFn<glm::vec2> testTexCoord0 = [](auto t1, auto t2) { EXPECT_EQ(t1, t2); };

  TestFn<glm::vec2> testTexCoord1 = [](auto t1, auto t2) { EXPECT_EQ(t1, t2); };

  TestFn<uint32_t> testIndex = [](auto t1, auto t2) { EXPECT_EQ(t1, t2); };
};

class MeshAttributeTestBase : public GLTFImporterTestBase {

public:
  template <GLTFTestAttribute TAttribute>
  GLTFTestScene
  createMeshWithInvalidPrimitiveTypes(const std::vector<int> &validTypes) {
    static std::array<int, 7> AllTypes{TINYGLTF_TYPE_SCALAR, TINYGLTF_TYPE_VEC2,
                                       TINYGLTF_TYPE_VEC3,   TINYGLTF_TYPE_VEC4,
                                       TINYGLTF_TYPE_MAT2,   TINYGLTF_TYPE_MAT3,
                                       TINYGLTF_TYPE_MAT4};

    std::vector<int> types;
    types.reserve(AllTypes.size());

    for (auto type : AllTypes) {
      bool found = false;
      for (size_t i = 0; i < validTypes.size() && !found; ++i) {
        found = validTypes.at(i) == type;
      }

      if (!found) {
        types.push_back(type);
      }
    }

    GLTFTestMesh mesh;
    for (auto type : types) {
      auto primitive = createCubePrimitive();
      primitive.get<TAttribute>().type = type;

      mesh.primitives.push_back(primitive);
    }

    GLTFTestScene scene;
    scene.meshes.push_back(mesh);

    return scene;
  }

  template <GLTFTestAttribute TAttribute>
  GLTFTestScene createMeshWithInvalidPrimitiveComponentTypes(
      const std::vector<int> &validTypes) {
    static std::array<int, 8> AllComponentTypes{
        TINYGLTF_COMPONENT_TYPE_BYTE,
        TINYGLTF_COMPONENT_TYPE_SHORT,
        TINYGLTF_COMPONENT_TYPE_INT,
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
        TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
        TINYGLTF_COMPONENT_TYPE_FLOAT,
        TINYGLTF_COMPONENT_TYPE_DOUBLE};

    std::vector<int> types;
    types.reserve(AllComponentTypes.size());

    for (auto type : AllComponentTypes) {
      bool found = false;
      for (size_t i = 0; i < validTypes.size() && !found; ++i) {
        found = validTypes.at(i) == type;
      }

      if (!found) {
        types.push_back(type);
      }
    }

    GLTFTestMesh mesh;
    for (auto type : types) {
      auto primitive = createCubePrimitive();
      primitive.get<TAttribute>().componentType = type;

      mesh.primitives.push_back(primitive);
    }

    GLTFTestScene scene;
    scene.meshes.push_back(mesh);

    return scene;
  }

  GLTFTestScene createValidMesh() {
    GLTFTestMesh mesh;

    auto primitive = createCubePrimitive();
    mesh.primitives.push_back(primitive);

    GLTFTestScene scene;
    scene.meshes.push_back(mesh);

    return scene;
  }

  liquid::Result<liquid::editor::UUIDMap> loadScene(GLTFTestScene &scene) {
    auto path = saveSceneGLTF(scene);
    return importer.loadFromPath(path, {});
  }

  void validateAttributes(GLTFTestScene &scene,
                          AttributeTestFns fns = AttributeTestFns{}) {
    EXPECT_EQ(assetCache.getRegistry().getMeshes().getAssets().size(), 1);
    EXPECT_EQ(scene.meshes.size(), 1);

    auto &gltfMesh = scene.meshes.at(0);

    const auto &meshAsset = assetCache.getRegistry().getMeshes().getAsset(
        liquid::MeshAssetHandle{1});

    EXPECT_EQ(meshAsset.data.geometries.size(), gltfMesh.primitives.size());
    for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
      const auto &g = meshAsset.data.geometries.at(gi);
      auto &p = gltfMesh.primitives.at(gi);

      EXPECT_EQ(g.vertices.size(), p.positions.data.size());
      if (p.normals.data.size() < g.vertices.size()) {
        p.normals.data.resize(g.vertices.size());
      }

      if (p.texCoords0.data.size() < g.vertices.size()) {
        p.texCoords0.data.resize(g.vertices.size());
      }

      if (p.texCoords1.data.size() < g.vertices.size()) {
        p.texCoords1.data.resize(g.vertices.size());
      }

      if (p.tangents.data.size() < g.vertices.size()) {
        p.tangents.data.resize(g.vertices.size());
      }

      if (p.indices.data.empty()) {
        p.indices.data.resize(g.indices.size());
      }

      for (size_t i = 0; i < g.vertices.size(); ++i) {
        auto &v = g.vertices.at(i);
        fns.testPosition(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
        fns.testNormal(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
        fns.testTangent(glm::vec4(v.tx, v.ty, v.tw, v.tz),
                        p.tangents.data.at(i));
        fns.testTexCoord0(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
        fns.testTexCoord1(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
      }

      EXPECT_EQ(g.indices.size(), p.indices.data.size());
      for (size_t i = 0; i < g.indices.size(); ++i) {
        fns.testIndex(g.indices.at(i), p.indices.data.at(i));
      }
    }
  }
};

class MeshPositionAttributeTest : public MeshAttributeTestBase {};

TEST_F(MeshPositionAttributeTest, DoesNotCreateMeshIfNoPositions) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).positions.data.clear();
  loadScene(scene);

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(MeshPositionAttributeTest,
       DoesNotCreateMeshIfInvalidPositionsAccessorType) {
  auto scene =
      createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::Positions>(
          {TINYGLTF_TYPE_VEC3});
  loadScene(scene);

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(MeshPositionAttributeTest,
       DoesNotCreateMeshIfInvalidPositionsAccessorComponentType) {
  auto scene = createMeshWithInvalidPrimitiveComponentTypes<
      GLTFTestAttribute::Positions>({TINYGLTF_COMPONENT_TYPE_FLOAT});

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(MeshPositionAttributeTest, LoadsIndicesFromGLTFPrimitiveIfValidIndices) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}

class MeshNormalAttributeTest : public MeshAttributeTestBase {
public:
  AttributeTestFns getInvalidTest() {
    AttributeTestFns fns{};
    fns.testNormal = [](auto n1, auto n2) { EXPECT_NE(n1, glm::vec3(0.0f)); };
    return fns;
  }
};

TEST_F(MeshNormalAttributeTest, GeneratedNormalsIfEmptyNormals) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).normals.data.clear();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshNormalAttributeTest,
       GeneratesNormalsIfNormalSizeIsNotEqualToVertexSize) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).normals.data.pop_back();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshNormalAttributeTest, GeneratesNormalsIfNormalAccessorTypeIsInvalid) {
  auto scene = createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::Normals>(
      {TINYGLTF_TYPE_VEC3});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshNormalAttributeTest,
       GeneratesNormalsIfNormalAccessorComponentTypeIsInvalid) {
  auto scene =
      createMeshWithInvalidPrimitiveComponentTypes<GLTFTestAttribute::Normals>(
          {TINYGLTF_COMPONENT_TYPE_FLOAT});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshNormalAttributeTest,
       LoadsNormalsFromGLTFPrimitiveIfValidNormalAccessor) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}

class MeshTexCoordAttributeTest : public MeshAttributeTestBase {
public:
  AttributeTestFns getInvalidTest(uint32_t index) {
    AttributeTestFns fns{};

    if (index == 0) {
      fns.testTexCoord0 = [](auto n1, auto n2) {
        EXPECT_EQ(n1, glm::vec2(0.0f));
      };
    }

    if (index == 1) {
      fns.testTexCoord1 = [](auto n1, auto n2) {
        EXPECT_EQ(n1, glm::vec2(0.0f));
      };
    }

    return fns;
  }
};

TEST_F(MeshTexCoordAttributeTest, SetsTexCoords0ToZeroIfEmptyTexCoords) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).texCoords0.data.clear();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(0));
}

TEST_F(MeshTexCoordAttributeTest,
       SetsTexCoords0ToZeroIfTexCoords0AccessorTypeIsInvalid) {
  auto scene =
      createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::TexCoords0>(
          {TINYGLTF_TYPE_VEC2});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(0));
}

TEST_F(MeshTexCoordAttributeTest,
       SetsTexCoords0ToZeroIfTexCoords0AccessorComponentTypeIsInvalid) {
  auto scene = createMeshWithInvalidPrimitiveComponentTypes<
      GLTFTestAttribute::TexCoords0>({TINYGLTF_COMPONENT_TYPE_FLOAT});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(0));
}

TEST_F(MeshTexCoordAttributeTest,
       LoadsTexCoordsFromGLTFPrimitiveIfValidTexCoords0Accessor) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}

TEST_F(MeshTexCoordAttributeTest, SetsTexCoords1ToZeroIfEmptyTexCoords) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).texCoords1.data.clear();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(1));
}

TEST_F(MeshTexCoordAttributeTest,
       SetsTexCoords0ToZeroIfTexCoords1AccessorTypeIsInvalid) {
  auto scene =
      createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::TexCoords1>(
          {TINYGLTF_TYPE_VEC2});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(1));
}

TEST_F(MeshTexCoordAttributeTest,
       SetsTexCoords0ToZeroIfTexCoords1AccessorComponentTypeIsInvalid) {
  auto scene = createMeshWithInvalidPrimitiveComponentTypes<
      GLTFTestAttribute::TexCoords1>({TINYGLTF_COMPONENT_TYPE_FLOAT});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest(1));
}

TEST_F(MeshTexCoordAttributeTest,
       LoadsTexCoordsFromGLTFPrimitiveIfValidTexCoords1Accessor) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}

class MeshTangentAttributeTest : public MeshAttributeTestBase {
public:
  AttributeTestFns getInvalidTest() {
    AttributeTestFns fns{};
    fns.testTangent = [](auto n1, auto n2) { EXPECT_NE(n1, glm::vec4(0.0f)); };
    return fns;
  }
};

TEST_F(MeshTangentAttributeTest, GeneratesTangentsIfEmptyTangents) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).tangents.data.clear();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshTangentAttributeTest,
       GeneratesTangentsIfTangentSizeIsNotEqualToVertexSize) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).tangents.data.pop_back();
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshTangentAttributeTest,
       GeneratesTangentsIfTangentAccessorTypeIsInvalid) {
  auto scene = createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::Tangents>(
      {TINYGLTF_TYPE_VEC4});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshTangentAttributeTest,
       GeneratesTangentsIfTangentAccessorComponentTypeIsInvalid) {
  auto scene =
      createMeshWithInvalidPrimitiveComponentTypes<GLTFTestAttribute::Tangents>(
          {TINYGLTF_COMPONENT_TYPE_FLOAT});
  loadScene(scene);
  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshTangentAttributeTest,
       LoadsTangentsFromGLTFPrimitiveIfValidTangentAccessor) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}

class MeshIndexAttributeTest : public MeshAttributeTestBase {
public:
  AttributeTestFns getInvalidTest() {
    static constexpr uint32_t NumVertices = 24u;

    AttributeTestFns fns{};
    fns.testIndex = [](auto n1, auto n2) { EXPECT_LT(n1, NumVertices); };
    return fns;
  }
};

TEST_F(MeshIndexAttributeTest, GeneratesIndicesIfEmptyIndices) {
  auto scene = createValidMesh();
  scene.meshes.at(0).primitives.at(0).indices.data.clear();
  loadScene(scene);

  validateAttributes(scene, getInvalidTest());
}

TEST_F(MeshIndexAttributeTest, DoesNotCreateMeshIfInvalidIndicesAccessorType) {
  auto scene = createMeshWithInvalidPrimitiveTypes<GLTFTestAttribute::Indices>(
      {TINYGLTF_TYPE_SCALAR});
  loadScene(scene);

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(MeshIndexAttributeTest,
       DoesNotCreateMeshIfInvalidIndicesAccessorComponentType) {
  auto scene =
      createMeshWithInvalidPrimitiveComponentTypes<GLTFTestAttribute::Indices>(
          {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
           TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
           TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT});

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(MeshIndexAttributeTest, LoadsIndicesFromGLTFPrimitiveIfValidIndices) {
  auto scene = createValidMesh();
  loadScene(scene);
  validateAttributes(scene);
}
