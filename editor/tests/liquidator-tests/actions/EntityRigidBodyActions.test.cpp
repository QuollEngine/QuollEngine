#include "liquid/core/Base.h"
#include "liquidator/actions/EntityRigidBodyActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateRigidBodyActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateRigidBodyActionTest,
                                EntityCreateRigidBody, RigidBody);
InitActionsTestSuite(EntityActionsTest, EntityCreateRigidBodyActionTest);

using EntityDeleteRigidBodyActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteRigidBodyActionTest,
                                EntityDeleteRigidBody, RigidBody);
InitActionsTestSuite(EntityActionsTest, EntityDeleteRigidBodyActionTest);

using EntitySetRigidBodyActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetRigidBodyActionTest,
                                EntitySetRigidBody, RigidBody, dynamicDesc.mass,
                                5.0f);

InitActionsTestSuite(EntityActionsTest, EntitySetRigidBodyActionTest);
