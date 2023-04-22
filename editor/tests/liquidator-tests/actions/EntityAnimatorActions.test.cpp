#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAnimatorActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityDeleteAnimatorActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteAnimatorActionTest,
                                EntityDeleteAnimator, Animator);
InitActionsTestSuite(EntityActionsTest, EntityDeleteAnimatorActionTest);
