#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityLightActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreatePointLightActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreatePointLightActionTest,
                                EntityCreatePointLight, PointLight);
InitActionsTestSuite(EntityActionsTest, EntityCreatePointLightActionTest);

using EntitySetPointLightActionTest = ActionTestBase;
InitDefaultUpdateComponentTests(EntitySetPointLightActionTest,
                                EntitySetPointLight, PointLight, color,
                                glm::vec4{0.3f});
InitActionsTestSuite(EntityActionsTest, EntitySetPointLightActionTest);

using EntityDeletePointLightActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeletePointLightActionTest,
                                EntityDeletePointLight, PointLight);
InitActionsTestSuite(EntityActionsTest, EntityDeletePointLightActionTest);
