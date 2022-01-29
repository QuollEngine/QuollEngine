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

TEST(KeyframeSequenceTest, GetLastKeyframeValueIfTimeIsBiggerThanLastKeyframe) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Step);
  sequence.addKeyframe(1.0f, glm::vec4{0.5f});

  EXPECT_EQ(sequence.getInterpolatedValue(5.0f), glm::vec4(0.5f));
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

TEST(KeyframeSequenceTest,
     GetInterpolatedKeyFrameValueBetweenTwoKeysOnLinearInterpolation) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Linear);

  sequence.addKeyframe(0.0f, glm::vec4{0.0f});
  sequence.addKeyframe(0.5f, glm::vec4{0.5f});
  sequence.addKeyframe(1.0f, glm::vec4{1.0f});

  for (float t = 0; t < 0.5f; t += 0.05f) {
    // 0 + (t - 0.0) * vec4(0.5 - 0.0) / (0.5 - 0.0) = t
    EXPECT_EQ(sequence.getInterpolatedValue(t), glm::vec4(t));
  }

  for (float t = 0.5f; t <= 1.0f; t += 0.05f) {
    // 0.5 + (t - 0.5) * vec4(1.0 - 0.0) / (1.0 - 0.0) = 0.5 + (1 - 0.5)
    EXPECT_EQ(sequence.getInterpolatedValue(t), glm::vec4(0.5f + (t - 0.5f)));
  }
}

TEST(KeyframeSequenceTest, GetLastItemValueIfTimeIsLargerThanLastTime) {
  liquid::KeyframeSequence sequence(SequenceTarget::Position,
                                    SequenceInterpolation::Linear);
  sequence.addKeyframe(0.0f, glm::vec4{0.0f});
  sequence.addKeyframe(0.5f, glm::vec4{0.5f});
  sequence.addKeyframe(1.0f, glm::vec4{3.0f});

  EXPECT_EQ(sequence.getInterpolatedValue(1.0f), glm::vec4(3.0f));
  EXPECT_EQ(sequence.getInterpolatedValue(2.0f), glm::vec4(3.0f));
}
