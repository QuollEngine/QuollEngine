#include "liquid/core/Base.h"
#include "liquidator/actions/EntityMeshActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityDeleteMeshActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteMeshActionTest, EntityDeleteMesh,
                                Mesh);
InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshActionTest);

using EntityDeleteSkinnedMeshActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteSkinnedMeshActionTest,
                                EntityDeleteSkinnedMesh, SkinnedMesh);
InitActionsTestSuite(EntityActionsTest, EntityDeleteSkinnedMeshActionTest);
