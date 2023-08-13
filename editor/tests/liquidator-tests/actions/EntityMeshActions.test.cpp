#include "liquid/core/Base.h"
#include "liquidator/actions/EntityMeshActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateMeshActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateMeshActionTest, EntityCreateMesh,
                                Mesh);
InitActionsTestSuite(EntityActionsTest, EntityCreateMeshActionTest);

using EntityUpdateMeshActionTest = ActionTestBase;
InitDefaultUpdateComponentTests(EntityUpdateMeshActionTest, EntitySetMesh, Mesh,
                                handle, liquid::MeshAssetHandle{25});
InitActionsTestSuite(EntityActionsTest, EntityUpdateMeshActionTest);

using EntityDeleteMeshActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteMeshActionTest, EntityDeleteMesh,
                                Mesh);
InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshActionTest);

// Skinned mesh
using EntityCreateSkinnedMeshActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateSkinnedMeshActionTest,
                                EntityCreateSkinnedMesh, SkinnedMesh);
InitActionsTestSuite(EntityActionsTest, EntityCreateSkinnedMeshActionTest);

using EntityUpdateSkinnedMeshActionTest = ActionTestBase;
InitDefaultUpdateComponentTests(EntityUpdateSkinnedMeshActionTest,
                                EntitySetSkinnedMesh, SkinnedMesh, handle,
                                liquid::SkinnedMeshAssetHandle{25});
InitActionsTestSuite(EntityActionsTest, EntityUpdateSkinnedMeshActionTest);

using EntityDeleteSkinnedMeshActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteSkinnedMeshActionTest,
                                EntityDeleteSkinnedMesh, SkinnedMesh);
InitActionsTestSuite(EntityActionsTest, EntityDeleteSkinnedMeshActionTest);
