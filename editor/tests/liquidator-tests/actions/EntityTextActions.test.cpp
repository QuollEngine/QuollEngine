#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTextActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateTextActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateTextActionTest, EntityCreateText,
                                Text);
InitActionsTestSuite(EntityActionsTest, EntityCreateTextActionTest);

using EntityDeleteTextActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteTextActionTest, EntityDeleteText,
                                Text);
InitActionsTestSuite(EntityActionsTest, EntityDeleteTextActionTest);

using EntitySetTextActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetTextActionTest, EntitySetText, Text,
                                text, "Hello world");

InitActionsTestSuite(EntityActionsTest, EntitySetTextActionTest);
