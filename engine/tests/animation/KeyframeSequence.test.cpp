#include "core/Base.h"
#include "animation/KeyframeSequence.h"

#include <gtest/gtest.h>

using SequenceTarget = liquid::KeyframeSequenceTarget;
using SequenceInterpolation = liquid::KeyframeSequenceInterpolation;

TEST(KeyframeSequenceTest, CreateSequenceForPosition) {

  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  EXPECT_EQ(sequence.getTarget(), liquid::KeyframeSequenceTarget::Position);
}

TEST(KeyframeSequenceTest, CreateSequenceForRotation) {
  liquid::KeyframeSequence sequence(SequenceTarget::Rotation,
                                    SequenceInterpolation::Step);

  EXPECT_EQ(sequence.getTarget(), SequenceTarget::Rotation);
}

TEST(KeyframeSequenceTest, CreateSequenceForScale) {
  liquid::KeyframeSequence sequence(SequenceTarget::Scale,
                                    SequenceInterpolation::Step);

  EXPECT_EQ(sequence.getTarget(), SequenceTarget::Scale);
}

TEST(KeyframeSequenceTest, CreateSequenceWithStepInterpolation) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  EXPECT_EQ(sequence.getInterpolation(), SequenceInterpolation::Step);
}

TEST(KeyframeSequenceDeathTest, ErrorOnAddKeyframeIfTimeIsLessThanZero) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  EXPECT_DEATH(
      {
        sequence.addKeyframe(-0.1f, glm::vec4{1.0f, 1.0f, 1.0f, 0.0f});
      },
      ".*");
}

TEST(KeyframeSequenceDeathTest, ErrorOnAddKeyframeIfTimeIsMoreThanOne) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  EXPECT_DEATH(
      {
        sequence.addKeyframe(1.1f, glm::vec4{1.0f, 1.0f, 1.0f, 0.0f});
      },
      ".*");
}

TEST(KeyframeSequenceTest, AddKeyframe) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  sequence.addKeyframe(0.5f, glm::vec4{1.0f, 1.0f, 1.0f, 0.0f});

  EXPECT_EQ(sequence.getKeyframeTime(0), 0.5f);
  EXPECT_EQ(sequence.getKeyframeValue(0), glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
}

TEST(KeyframeSequenceDeathTest,
     ErrorOnInterpolatedValueIfSequenceHasNoKeyframes) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  EXPECT_DEATH({ sequence.getInterpolatedValue(0.0f); }, ".*");
}

TEST(KeyframeSequenceTest, GetPreviousKeyframeValueOnStepInterpolation) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);

  sequence.addKeyframe(0.0f, glm::vec4{0.0f});
  sequence.addKeyframe(0.5f, glm::vec4{0.5f});
  sequence.addKeyframe(1.0f, glm::vec4{1.0f});

  EXPECT_EQ(sequence.getInterpolatedValue(0.0f), glm::vec4(0.0f));
  EXPECT_EQ(sequence.getInterpolatedValue(0.2f), glm::vec4(0.0f));
  EXPECT_EQ(sequence.getInterpolatedValue(0.6f), glm::vec4(0.5f));
  EXPECT_EQ(sequence.getInterpolatedValue(0.8f), glm::vec4(0.5f));
  EXPECT_EQ(sequence.getInterpolatedValue(1.0f), glm::vec4(1.0f));
}

TEST(KeyframeSequenceTest, GetLastKeyframeValueIfTimeIsBiggerThanLastKeyframe) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);
  sequence.addKeyframe(1.0f, glm::vec4{0.5f});

  EXPECT_EQ(sequence.getInterpolatedValue(5.0f), glm::vec4(0.5f));
}
