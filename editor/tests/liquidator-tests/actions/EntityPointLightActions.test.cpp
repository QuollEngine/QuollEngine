#include "liquid/core/Base.h"
#include "liquidator/actions/EntityLightActions.h"

#include "liquidator-tests/Testing.h"
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
