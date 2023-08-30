#include "quoll/core/Base.h"
#include "liquidator/actions/EntityNameActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntitySetNameActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetNameActionTest, EntitySetName, Name,
                                name, "Hello world");

InitActionsTestSuite(EntityActionsTest, EntitySetNameActionTest);
