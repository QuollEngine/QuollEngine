#include "quoll/core/Base.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/editor/actions/EntityTransformActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetLocalTransformContinousActionTest = ActionTestBase;

TEST_F(EntitySetLocalTransformContinousActionTest,
       ExecutorSetsFinalTransformForEntity) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::LocalTransform startTransform{glm::vec3{2.5f}};
  quoll::LocalTransform finalTransform{glm::vec3{5.5f}};

  quoll::editor::EntitySetLocalTransformContinuous action(entity,
                                                          startTransform);
  action.setNewComponent(finalTransform);

  auto res = action.onExecute(state, assetRegistry);
  ASSERT_TRUE(entity.has<quoll::LocalTransform>());
  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3(5.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetLocalTransformContinousActionTest,
       UndoSetsStartTransformForEntityWhereStartTransformIsDefined) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::LocalTransform startTransform{glm::vec3{2.5f}};
  quoll::LocalTransform finalTransform{glm::vec3{5.5f}};

  quoll::editor::EntitySetLocalTransformContinuous action(entity,
                                                          startTransform);

  auto res = action.onUndo(state, assetRegistry);
  ASSERT_TRUE(entity.has<quoll::LocalTransform>());
  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetLocalTransformContinousActionTest,
       UndoRemovesTransformComponentFromEntityIfStartTransformIsNotDefined) {
  auto entity = state.scene.entityDatabase.entity();

  entity.set<quoll::LocalTransform>({});
  quoll::editor::EntitySetLocalTransformContinuous action(entity, std::nullopt);

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetLocalTransformContinousActionTest,
       UndoRemovesWorldTransfromFromEntityOnUndoIfStartTransformIsNotDefined) {
  auto entity = state.scene.entityDatabase.entity();

  entity.set<quoll::LocalTransform>({});
  entity.set<quoll::WorldTransform>({});
  quoll::editor::EntitySetLocalTransformContinuous action(entity, std::nullopt);

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
  EXPECT_FALSE(entity.has<quoll::WorldTransform>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsTrueIfActionHasFinalTransform) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntitySetLocalTransformContinuous action(
      entity, {}, quoll::LocalTransform{glm::vec3{5.5f}});

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsFalseIfActionDoesNotHaveFinalTransform) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntitySetLocalTransformContinuous action(entity, {});

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}
