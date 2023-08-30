#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityNameActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntitySetNameActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetNameActionTest, EntitySetName, Name,
                                name, "Hello world");

InitActionsTestSuite(EntityActionsTest, EntitySetNameActionTest);
