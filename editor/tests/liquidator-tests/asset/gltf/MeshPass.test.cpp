#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquidator-tests/Testing.h"
#include "liquidator/asset/GLTFImporter.h"

#include "GLTFImporterTestBase.h"

class GLTFImporterMeshTest : public GLTFImporterTestBase {};

// Mesh vertex position test
TEST_F(GLTFImporterMeshTest, DoesNotCreateMeshIfNoPositionAttribute) {
  GLTFTestMesh mesh;
  auto primitive = createCubePrimitive();
  primitive.positions.data.clear();

  mesh.primitives.push_back(primitive);

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(GLTFImporterMeshTest, DoesNotCreateMeshIfPositionAccessorTypeIsNotVec3) {
  std::array<int, 6> types{TINYGLTF_TYPE_SCALAR, TINYGLTF_TYPE_VEC2,
                           TINYGLTF_TYPE_VEC4,   TINYGLTF_TYPE_MAT2,
                           TINYGLTF_TYPE_MAT3,   TINYGLTF_TYPE_MAT4};

  GLTFTestMesh mesh;
  for (auto type : types) {
    auto primitive = createCubePrimitive();
    primitive.positions.type = type;
    mesh.primitives.push_back(primitive);
  }

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(GLTFImporterMeshTest,
       DoesNotCreateMeshIfPositionAccessorComponentTypeIsNotFloat) {
  std::array<int, 7> componentTypes{TINYGLTF_COMPONENT_TYPE_BYTE,
                                    TINYGLTF_COMPONENT_TYPE_SHORT,
                                    TINYGLTF_COMPONENT_TYPE_INT,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
                                    TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
                                    TINYGLTF_COMPONENT_TYPE_DOUBLE};

  GLTFTestMesh mesh;
  for (auto componentType : componentTypes) {
    auto primitive = createCubePrimitive();
    primitive.positions.componentType = componentType;

    mesh.primitives.push_back(primitive);
  }

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_TRUE(assetCache.getRegistry().getMeshes().getAssets().empty());
}

TEST_F(GLTFImporterMeshTest, CreatesMeshIfPositionAccessorIsValid) {
  GLTFTestMesh mesh;
  auto primitive = createCubePrimitive();
  mesh.primitives.push_back(primitive);

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  // Get first asset
  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  // TODO: Test all vertices
}

template <class T> class MeshAttributeTest : public GLTFImporterTestBase {};

TYPED_TEST_SUITE_P(MeshAttributeTest);

template <class T>
class MeshVertexAttributeTest : public GLTFImporterTestBase {};

TYPED_TEST_SUITE_P(MeshVertexAttributeTest);

TYPED_TEST_P(MeshAttributeTest, CreatesMeshWithoutAttributeIfNotProvided) {
  liquid::AssetCache assetCache(CachePath, false);
  liquidator::GLTFImporter importer(assetCache);

  GLTFTestMesh mesh;
  GLTFTestPrimitive primitive = createCubePrimitive();
  primitive.get<typename TypeParam::Type>().data.clear();

  mesh.primitives.push_back(primitive);
  GLTFTestScene scene;
  scene.meshes.push_back(mesh);
  auto path = saveSceneGLTF(scene);

  for (auto &p : mesh.primitives) {
    TypeParam::PopulateExpectOnInvalid::call(p.get<typename TypeParam::Type>(),
                                             p);
  }

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  EXPECT_EQ(meshAsset.data.geometries.size(), mesh.primitives.size());
  for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
    const auto &g = meshAsset.data.geometries.at(gi);
    const auto &p = mesh.primitives.at(gi);

    EXPECT_EQ(g.vertices.size(), p.positions.data.size());
    for (size_t i = 0; i < g.vertices.size(); ++i) {
      auto &v = g.vertices.at(i);
      EXPECT_EQ(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
      EXPECT_EQ(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
      EXPECT_EQ(glm::vec4(v.tx, v.ty, v.tw, v.tz), p.tangents.data.at(i));
      EXPECT_EQ(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
      EXPECT_EQ(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
    }

    if constexpr (std::is_base_of<typename TypeParam::Type,
                                  GLTFTestAttribute::Indices>()) {
      EXPECT_TRUE(g.indices.empty());
    }
  }
}

TYPED_TEST_P(MeshAttributeTest,
             CreatesMeshWithoutAttributeIfInvalidComponentType) {
  liquid::AssetCache assetCache(CachePath, false);
  liquidator::GLTFImporter importer(assetCache);

  static std::array<int, 8> AllComponentTypes{
      TINYGLTF_COMPONENT_TYPE_BYTE,
      TINYGLTF_COMPONENT_TYPE_SHORT,
      TINYGLTF_COMPONENT_TYPE_INT,
      TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
      TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
      TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
      TINYGLTF_COMPONENT_TYPE_FLOAT,
      TINYGLTF_COMPONENT_TYPE_DOUBLE};

  std::vector<int> componentTypes;
  componentTypes.reserve(AllComponentTypes.size());

  for (auto type : AllComponentTypes) {
    bool found = false;
    for (size_t i = 0; i < TypeParam::ComponentTypes::Data.size() && !found;
         ++i) {
      found = TypeParam::ComponentTypes::Data.at(i) == type;
    }

    if (!found) {
      componentTypes.push_back(type);
    }
  }

  GLTFTestMesh mesh;
  for (auto componentType : componentTypes) {
    auto primitive = createCubePrimitive();
    primitive.get<typename TypeParam::Type>().componentType = componentType;

    mesh.primitives.push_back(primitive);
  }

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  for (auto &p : mesh.primitives) {
    TypeParam::PopulateExpectOnInvalid::call(p.get<typename TypeParam::Type>(),
                                             p);
  }

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  EXPECT_EQ(meshAsset.data.geometries.size(), mesh.primitives.size());
  for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
    const auto &g = meshAsset.data.geometries.at(gi);
    const auto &p = mesh.primitives.at(gi);

    EXPECT_EQ(g.vertices.size(), p.positions.data.size());
    for (size_t i = 0; i < g.vertices.size(); ++i) {
      auto &v = g.vertices.at(i);
      EXPECT_EQ(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
      EXPECT_EQ(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
      EXPECT_EQ(glm::vec4(v.tx, v.ty, v.tw, v.tz), p.tangents.data.at(i));
      EXPECT_EQ(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
      EXPECT_EQ(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
    }

    if constexpr (std::is_base_of<typename TypeParam::Type,
                                  GLTFTestAttribute::Indices>()) {
      EXPECT_TRUE(g.indices.empty());
    }
  }
}

TYPED_TEST_P(MeshAttributeTest, CreatesMeshWithoutAttributeIfInvalidType) {
  liquid::AssetCache assetCache(CachePath, false);
  liquidator::GLTFImporter importer(assetCache);

  static std::array<int, 7> AllTypes{TINYGLTF_TYPE_SCALAR, TINYGLTF_TYPE_VEC2,
                                     TINYGLTF_TYPE_VEC3,   TINYGLTF_TYPE_VEC4,
                                     TINYGLTF_TYPE_MAT2,   TINYGLTF_TYPE_MAT3,
                                     TINYGLTF_TYPE_MAT4};

  std::vector<int> types;
  types.reserve(AllTypes.size());

  for (auto type : AllTypes) {
    bool found = false;
    for (size_t i = 0; i < TypeParam::Types::Data.size() && !found; ++i) {
      found = TypeParam::Types::Data.at(i) == type;
    }

    if (!found) {
      types.push_back(type);
    }
  }

  GLTFTestMesh mesh;
  for (auto type : types) {
    auto primitive = createCubePrimitive();
    primitive.get<typename TypeParam::Type>().type = type;

    mesh.primitives.push_back(primitive);
  }

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  for (auto &p : mesh.primitives) {
    TypeParam::PopulateExpectOnInvalid::call(p.get<typename TypeParam::Type>(),
                                             p);
  }

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  EXPECT_EQ(meshAsset.data.geometries.size(), mesh.primitives.size());
  for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
    const auto &g = meshAsset.data.geometries.at(gi);
    const auto &p = mesh.primitives.at(gi);

    EXPECT_EQ(g.vertices.size(), p.positions.data.size());
    for (size_t i = 0; i < g.vertices.size(); ++i) {
      auto &v = g.vertices.at(i);
      EXPECT_EQ(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
      EXPECT_EQ(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
      EXPECT_EQ(glm::vec4(v.tx, v.ty, v.tw, v.tz), p.tangents.data.at(i));
      EXPECT_EQ(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
      EXPECT_EQ(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
    }

    if constexpr (std::is_base_of<typename TypeParam::Type,
                                  GLTFTestAttribute::Indices>()) {
      EXPECT_TRUE(g.indices.empty());
    }
  }
}

TYPED_TEST_P(
    MeshVertexAttributeTest,
    CreatesMeshWithoutAttributeIfAttributeSizeIsDifferentFromPositionSize) {
  liquid::AssetCache assetCache(CachePath, false);
  liquidator::GLTFImporter importer(assetCache);

  GLTFTestMesh mesh;
  auto primitive = createCubePrimitive();
  primitive.get<typename TypeParam::Type>().data.pop_back();

  mesh.primitives.push_back(primitive);

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);

  for (auto &p : mesh.primitives) {
    TypeParam::PopulateExpectOnInvalid::call(p.get<typename TypeParam::Type>(),
                                             p);
  }

  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  EXPECT_EQ(meshAsset.data.geometries.size(), mesh.primitives.size());
  for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
    const auto &g = meshAsset.data.geometries.at(gi);
    const auto &p = mesh.primitives.at(gi);

    EXPECT_EQ(g.vertices.size(), p.positions.data.size());
    for (size_t i = 0; i < g.vertices.size(); ++i) {
      auto &v = g.vertices.at(i);
      EXPECT_EQ(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
      EXPECT_EQ(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
      EXPECT_EQ(glm::vec4(v.tx, v.ty, v.tw, v.tz), p.tangents.data.at(i));
      EXPECT_EQ(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
      EXPECT_EQ(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
    }

    if constexpr (std::is_base_of<typename TypeParam::Type,
                                  GLTFTestAttribute::Indices>()) {
      EXPECT_TRUE(g.indices.empty());
    }
  }
}

TYPED_TEST_P(MeshAttributeTest, CreatesMeshWithAttributeIfValid) {
  liquid::AssetCache assetCache(CachePath, false);
  liquidator::GLTFImporter importer(assetCache);

  GLTFTestMesh mesh;

  auto primitive = createCubePrimitive();

  mesh.primitives.push_back(primitive);

  GLTFTestScene scene;
  scene.meshes.push_back(mesh);

  auto path = saveSceneGLTF(scene);
  auto res = importer.loadFromPath(path, CachePath);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());

  EXPECT_FALSE(assetCache.getRegistry().getMeshes().getAssets().empty());

  const auto &meshAsset =
      assetCache.getRegistry().getMeshes().getAsset(liquid::MeshAssetHandle{1});

  EXPECT_EQ(meshAsset.data.geometries.size(), mesh.primitives.size());
  for (size_t gi = 0; gi < meshAsset.data.geometries.size(); ++gi) {
    const auto &g = meshAsset.data.geometries.at(gi);
    const auto &p = mesh.primitives.at(gi);

    EXPECT_EQ(g.vertices.size(), p.positions.data.size());
    for (size_t i = 0; i < g.vertices.size(); ++i) {
      auto &v = g.vertices.at(i);
      EXPECT_EQ(glm::vec3(v.x, v.y, v.z), p.positions.data.at(i));
      EXPECT_EQ(glm::vec3(v.nx, v.ny, v.nz), p.normals.data.at(i));
      EXPECT_EQ(glm::vec4(v.tx, v.ty, v.tw, v.tz), p.tangents.data.at(i));
      EXPECT_EQ(glm::vec2(v.u0, v.v0), p.texCoords0.data.at(i));
      EXPECT_EQ(glm::vec2(v.u1, v.v1), p.texCoords1.data.at(i));
    }

    EXPECT_EQ(g.indices.size(), p.indices.data.size());
    for (size_t i = 0; i < g.indices.size(); ++i) {
      EXPECT_EQ(g.indices.at(i), p.indices.data.at(i));
    }
  }
}

REGISTER_TYPED_TEST_SUITE_P(MeshAttributeTest,
                            CreatesMeshWithoutAttributeIfNotProvided,
                            CreatesMeshWithoutAttributeIfInvalidComponentType,
                            CreatesMeshWithoutAttributeIfInvalidType,
                            CreatesMeshWithAttributeIfValid);

REGISTER_TYPED_TEST_SUITE_P(
    MeshVertexAttributeTest,
    CreatesMeshWithoutAttributeIfAttributeSizeIsDifferentFromPositionSize);

template <int... TValues> struct ValidValues {
  static constexpr std::array<int, sizeof...(TValues)> Data{TValues...};
};

template <class T, class TComponentTypes, class TTypes,
          class TPopulateExpectOnInvalid>
struct AttributeContainer {
  using Type = T;
  using ComponentTypes = TComponentTypes;
  using Types = TTypes;
  using PopulateExpectOnInvalid = TPopulateExpectOnInvalid;
};

template <class T> struct DefaultZeroFn {
  static void call(GLTFTestBufferData<T> &data, GLTFTestPrimitive &p) {
    data.fillWithZeros(p.positions.data.size());
  }
};

template <class T> struct NoopFn {
  static void call(GLTFTestBufferData<T> &data, GLTFTestPrimitive &p) {
    // do nothing
  }
};

using NormalAttribute = AttributeContainer<
    GLTFTestAttribute::Normals, ValidValues<TINYGLTF_COMPONENT_TYPE_FLOAT>,
    ValidValues<TINYGLTF_TYPE_VEC3>, DefaultZeroFn<glm::vec3>>;
using TangentAttribute = AttributeContainer<
    GLTFTestAttribute::Tangents, ValidValues<TINYGLTF_COMPONENT_TYPE_FLOAT>,
    ValidValues<TINYGLTF_TYPE_VEC4>, DefaultZeroFn<glm::vec4>>;

using TexCoords0Attribute = AttributeContainer<
    GLTFTestAttribute::TexCoords0, ValidValues<TINYGLTF_COMPONENT_TYPE_FLOAT>,
    ValidValues<TINYGLTF_TYPE_VEC2>, DefaultZeroFn<glm::vec2>>;

using TexCoords1Attribute = AttributeContainer<
    GLTFTestAttribute::TexCoords0, ValidValues<TINYGLTF_COMPONENT_TYPE_FLOAT>,
    ValidValues<TINYGLTF_TYPE_VEC2>, DefaultZeroFn<glm::vec2>>;

using IndexAttribute =
    AttributeContainer<GLTFTestAttribute::Indices,
                       ValidValues<TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
                                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT>,
                       ValidValues<TINYGLTF_TYPE_SCALAR>, NoopFn<uint32_t>>;

using MeshAttributeTestTypes =
    ::testing::Types<NormalAttribute, TangentAttribute, TexCoords0Attribute,
                     TexCoords1Attribute, IndexAttribute>;

using MeshVertexAttributeTestTypes =
    ::testing::Types<NormalAttribute, TangentAttribute, TexCoords0Attribute,
                     TexCoords1Attribute>;

INSTANTIATE_TYPED_TEST_SUITE_P(GLTFImporterTest, MeshAttributeTest,
                               MeshAttributeTestTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(GLTFImporterTest, MeshVertexAttributeTest,
                               MeshVertexAttributeTestTypes);
